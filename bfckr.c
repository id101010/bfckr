/*
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>
#include<errno.h>
#include<ctype.h>
#include<stdbool.h>
#include<unistd.h>

// Defines
#define MEMORY_SIZE     1000 // length of the band tape in bytes
#define MAX_INPUT_SIZE  1000 // maximal number of input bytes 
#define clear()         printf("\033[H\033[J") // clear the screen

// Struct which holds the brainfuck code, the bandtape and some helpervariables
typedef struct bf_code_s {
    char memory[MEMORY_SIZE];   // Memory initialized
    size_t mp;                  // Memory pointer
    char code[MAX_INPUT_SIZE];  // Input buffer for the bf code
    size_t ip;                  // Instruction pointer
    bool debug;                 // debug flag
} bf_code_t;

// Prototypes
static void bfuck_debugger(bf_code_t *bf);
static void print_sourceviewer(bf_code_t *bf);
static void print_memoryviewer(bf_code_t *bf);
static void init_bf_object(bf_code_t *bf);
static bool is_brainfuck(char c);
static char *colorize(char c);
int getopt(int argc, char * const argv[], const char *optstring);

// Globals
extern char *optarg;
extern int optind, opterr, optopt;
char output_buffer[MEMORY_SIZE];
size_t n_output_buffer = 0;
char *colortheme[] = {
    "\e[1;34m<\e[0m", // blue <
    "\e[1;34m>\e[0m", // blue >
    "\e[1;33m[\e[0m", // yellow [
    "\e[1;33m]\e[0m", // yellow ]
    "\e[1;32m+\e[0m", // green +
    "\e[1;32m-\e[0m", // green -
    "\e[1;31m.\e[0m", // red .
    "\e[1;31m,\e[0m", // red ,
    "\e[1;31m#\e[0m", // red #
};

/**
 * @brief       Colorize the ouput string using a colorscheme table
 * @type        static
 * @param[in]   Single character to be colorized
 * @return      Colorized string
 **/
static char *colorize(char c)
{
    char *cs = 0; // colorstring

    switch(c) {
    case '>':
        cs = colortheme[0];
        break;
    case '<':
        cs = colortheme[1];
        break;
    case '[':
        cs = colortheme[2];
        break;
    case ']':
        cs = colortheme[3];
        break;
    case '+':
        cs = colortheme[4];
        break;
    case '-':
        cs = colortheme[5];
        break;
    case '.':
        cs = colortheme[6];
        break;
    case ',':
        cs = colortheme[7];
        break;
    case '#':
        cs = colortheme[8];
        break;
    default:
        ;
        break;
    }

    return cs;
}

/**
 * @brief       Initialize a brainfuck object by setting all values to zero
 * @type        static
 * @param[in]   Pointer to a bf_code_t type
 * @return      void
 **/
static void init_bf_object(bf_code_t *bf)
{
    bf->mp = 0; // set data pointer to zero
    bf->ip = 0; // set instruction pointer to zero
    bf->debug = false; // reset the debug flag

    for(size_t i = 0; i < MEMORY_SIZE; i++) {
        bf->memory[i] = 0;
    }

    for(size_t i = 0; i < MAX_INPUT_SIZE; i++) {
        bf->code[i] = 0;
    }

    for(size_t i = 0; i < MEMORY_SIZE; i++) {
        output_buffer[i] = 0;
    }

}

/**
 * @brief       Tests if a single character contains a valid brainfuck instruction
 * @type        static
 * @param[in]   char c containing a brainfuck instruction to test
 * @return      bool which is true if the instruction is in brainfuck set or false if not
 **/
static bool is_brainfuck(char c)
{
    return (c=='+') || (c=='-') || (c=='>') || (c=='<') || (c=='.') || (c==',') || (c=='[') || (c==']' || (c=='#'));
}

/**
 * @brief       Error handler function wich shuts down the application and prints to stderr
 * @type        static
 * @param[in]   Message string
 * @return      void
 **/
static void die(const char *message)
{
    if(errno) {
        perror(message);
    } else {
        printf("[\e[1;31m:(\e[0m] Error: %s\n", message);
    }

    exit(EXIT_FAILURE);
}

/**
 * @brief       Prints the current ouput buffered in global output_buffer
 * @type        static
 * @param[in]   void
 * @return      void
 **/
static void print_output()
{
    printf("\nOutput viewer:                                            \n");
    printf("------------------------------------------------------------\n"); // 61 dashes
    // print the output buffer
    for(size_t i = 0; i < n_output_buffer; i++) {
        printf("\e[1;31m%c\e[0m", output_buffer[i]);
    }
    printf("\n------------------------------------------------------------\n");
}

/**
 * @brief       Prints the bf source code at the current location
 * @type        static
 * @param[in]   Pointer to a bf_code_t type
 * @return      void
 **/
static void print_sourceviewer(bf_code_t *bf)
{
    int ip = (int)bf->ip; // save instruction pointer

    printf("\nSource viewer:                                            \n");
    printf("------------------------------------------------------------\n"); // 61 dashes

    // print 30 valid chars before $ip and 30 after
    for(int i=(ip-30); i<(ip+30); i++) {
        printf("%s",((i<0 || i>=strlen(bf->code)) ? " ": colorize(bf->code[i])));
    }

    printf("\n                              \e[1;31m^\e[0m                             \n");
    printf("                              \e[1;31mip=%d\e[0m                         \n", ip);
    printf("------------------------------------------------------------\n");
}

/**
 * @brief       Prints a memory dump at the current memory location
 * @type        static
 * @param[in]   Pointer to a bf_code_t type
 * @return      void
 **/
void print_memoryviewer(bf_code_t *bf)
{
    int mp = (int)bf->mp; // save the current memory pointer

    printf("\nMemory viewer:                                            \n");
    printf("------------------------------------------------------------\n"); // 61 dashes

    // print the memory cells
    for(int i=(mp-7); i<(mp+8); i++) {
        printf("%03d ", ((i<0 || i>MEMORY_SIZE) ? 0 : bf->memory[i]));
    }

    printf("\n                              \e[1;31m^\e[0m                             \n");
    printf("                              \e[1;31mmp=%d\e[0m                         \n", mp);


    // print the adresses beneath the memory cells
    for(int i=(mp-7); i<(mp+8); i++) {

        if((i<0) || i>MEMORY_SIZE) {
            printf("%03d ", MEMORY_SIZE+i);
        } else {
            printf("%03d ", i);
        }
    }


    //"249 250 251 252 253 254 000 001 002 003 004 005 006 007 008\n"

    printf("\n------------------------------------------------------------\n");
}

/**
 * @brief       Pauses the program flow and prints information
 * @type        static
 * @param[in]   Pointer to a bf_code_t type
 * @return      void
 **/
static void bfuck_debugger(bf_code_t *bf)
{
    clear(); // clear terminal

    printf("[ENTER]: single step [c]: continue\n");

    print_sourceviewer(bf);
    print_memoryviewer(bf);
    print_output();

    switch(getchar()) {
    case 'c':
        // continue
        bf->debug = false;
        break;
    default:
        // single step
        break;
    }
}

/**
 * @brief       Parses and executes a brainfuck expression using a simple state machine
 * @type        static
 * @param[in]   Pointer to a bf_code_t type
 * @return      void
 **/
static void bfuck_execute(bf_code_t *bf)
{
    int loop = 0;

    for(size_t i = bf->ip; bf->code[i] != 0; bf->ip=i++) { // where i is the instruction pointer

        if(bf->debug) {
            bfuck_debugger(bf);
        }

        switch(bf->code[i]) {
        case '>':
            if(bf->mp >= MEMORY_SIZE) { // prevent overrun
                bf->mp = 0;

            } else {
                ++(bf->mp); // increment memory pointer
            }
            break;

        case '<':
            if(bf->mp < 0) { // prevent underun
                bf->mp = MEMORY_SIZE-1;
            } else {
                --(bf->mp); // decrement memory pointer
            }
            break;

        case '+':
            ++(bf->memory[bf->mp]); // increment byte at memory pointer
            break;

        case '-':
            --(bf->memory[bf->mp]); // decrement byte at memory pointer
            break;

        case '.':
            putchar(bf->memory[bf->mp]); // output the byte at memory pointer
            (n_output_buffer < MEMORY_SIZE) ? output_buffer[++n_output_buffer] = bf->memory[bf->mp] : 0; // write to output buffer for debugging
            break;

        case ',':
            bf->memory[bf->mp] = getchar(); // accept one byte of input and store it at memory pointer
            break;

        case '[':
            if(bf->memory[bf->mp] == 0) { // if the byte at the memory pointer is zero
                // jump forward to the command after the next ]
                loop = 1;
                while(loop > 0) { // count nested loops and make sure to get the matching ]
                    bf->ip = i++; // save current instruction pointer and increment it
                    if(bf->code[i] == '[') {
                        loop++;
                    }
                    if(bf->code[i] == ']') {
                        loop--;
                    }
                }
            } else {
                continue; // increment instruction pointer
            }
            break;

        case ']':
            if(bf->memory[bf->mp] != 0) { // if the byte at the memory pointer is nonzero
                // jump back to the command after the matching [
                loop = 1;
                while(loop > 0) { // count nested loops and make sure to get the matching [
                    bf->ip = i--; // save current instruction pointer and increment it
                    if(bf->code[i] == '[') {
                        loop--;
                    }
                    if(bf->code[i] == ']') {
                        loop++;
                    }
                }

            } else {
                continue; // increment instruction pointer
            }
            break;

        case '#':
            // Breakpoint reached, start debugger ...
            bf->debug = true;
            break;

        default:
            // Do nothing
            break;
        }
    }
}

/**
 * @brief       Main function which handles the argument parsing
 * @type        static
 * @param[in]   int argc        Argument counter
 * @param[in]   char **argv     Argument vector
 * @return      int Exitcode    Exitcode of the application
 **/
int main(int argc, char* argv[])
{
    // vars
    FILE *fp;
    int i = 0;
    char c, option;
    bf_code_t bf;

    // initialize bf object
    init_bf_object(&bf);

    // optparsing ahead
    while((option = getopt(argc, argv, "hde:f:")) >= 0) {
        switch(option) {
        case 'h': // show help
            printf("Usage: %s [OPTION] [FILE]\n", argv[0]);
            printf("-h\t Show this help.\n");
            printf("-d\t Enable debugger.\n");
            printf("-f\t Execute brainfuck code given as file.\n");
            printf("-e\t Execute brainfuck code given as string.\n");
            exit(EXIT_SUCCESS);
            break;
        case 'e': // read input string
            // copy optarg to the code register
            memcpy(bf.code, optarg, strlen(optarg));
            break;
        case 'f': // file input
            // try to open file
            if((fp = fopen(optarg, "r")) == NULL) {
                die("Couldn't open file.");
            }
            // read the file and store it in the input buffer
            while((c = getc(fp)) != EOF) {
                if(is_brainfuck(c)) {
                    bf.code[i++]  = c;
                }
            }
            // close file after reading
            fclose(fp);
            break;
        case 'd': // set to use debugger
            bf.debug = true;
            break;
        default:
            die("Invalid argument, see help.");
        }
    }

    // try to interpret it
    bfuck_execute(&bf);

    // test
    printf("\n");

    // exit
    exit(EXIT_SUCCESS);
}
