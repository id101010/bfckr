/*
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<errno.h>
#include<assert.h>
#include<ctype.h>
#include<stdbool.h>

// Defines
#define MEMORY_SIZE     10000
#define MAX_INPUT_SIZE  10000
#define clear()         printf("\033[H\033[J")

// Struct which holds the brainfuck code, the bandtape and some helpervariables
typedef struct bf_code_s {
    char memory[MEMORY_SIZE];   // Memory initialized
    size_t mp;                  // Memory pointer
    char code[MAX_INPUT_SIZE];  // Input buffer for the bf code
    size_t ip;                  // Instruction pointer
    bool debug;                 // debug flag
} bf_code_t;

// Prototypes
void bfuck_parser(bf_code_t *bf);
void bfuck_debugger(bf_code_t *bf);
void print_sourceviewer(bf_code_t *bf);
void print_memoryviewer(bf_code_t *bf);
void init_bf_object(bf_code_t *bf);
bool is_brainfuck(char c);

/* initialize bf object */
void init_bf_object(bf_code_t *bf)
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
}

bool is_brainfuck(char c)
{
    return (c=='+') || (c=='-') || (c=='>') || (c=='<') || (c=='.') || (c==',') || (c=='[') || (c==']');
}

/* Error handler */
void die(const char *message)
{
    if(errno) {
        perror(message);
    } else {
        printf("[:(] Error: %s\n", message);
    }

    exit(EXIT_FAILURE);
}

/* Prints the bf source at the current location */
void print_sourceviewer(bf_code_t *bf)
{
    int ip = (int)bf->ip; // save instruction pointer

    printf("\nSource viewer:                                            \n");
    printf("------------------------------------------------------------\n"); // 61 dashes

    // print 30 valid chars before $ip and 30 after
    for(int i=(ip-30); i<(ip+30); i++) {
        putchar((i<0 || i>MAX_INPUT_SIZE) ? ' ' : bf->code[i]);
    }

    printf("\n");
    printf("                              ^                             \n");
    printf("                              ip=%d                         \n", ip);
    printf("------------------------------------------------------------\n");
}

/* Prints memory information at the current memory location */
void print_memoryviewer(bf_code_t *bf)
{
    int mp = (int)bf->mp; // save the current memory pointer

    printf("\nMemory viewer:                                            \n");
    printf("------------------------------------------------------------\n"); // 61 dashes

    // print the memory cells
    for(int i=(mp-7); i<(mp+8); i++) {
        printf("%03d ", ((i<0 || i>MEMORY_SIZE) ? 0 : bf->memory[i]));
    }

    printf("\n                              ^                             \n");
    printf("                              mp=%d                         \n", mp);


    // print the adresses beneath the memory cells
    for(int i=(mp-7); i<(mp+8); i++) {
        printf("%03d ", ((i<0 || i>MEMORY_SIZE) ? 0 : abs(i-mp)));
    }


    //"249 250 251 252 253 254 000 001 002 003 004 005 006 007 008\n"

    printf("\n------------------------------------------------------------\n");
}

/* Pauses the program flow and prints information */
void bfuck_debugger(bf_code_t *bf) //char *bf_source_input, int instructionpointer)
{
    clear(); // clear terminal

    printf("[s]: single step [c]: continue\n");

    print_sourceviewer(bf);
    print_memoryviewer(bf);

    switch(getchar()) {
    case 's':
        // single step
        break;
    case 'c':
        // continue
        bf->debug = false;
        break;
    }
}

/* Parses and executes a brainfuck expression */
void bfuck_execute(bf_code_t *bf)
{
    int loop = 0;

    for(size_t i = bf->ip; bf->code[i] != 0; bf->ip=i++) { // where i is the instruction pointer
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

        bfuck_debugger(bf);
    }
}

int main(int argc, char* argv[])
{
    FILE *fp;
    int i = 0;
    int c;
    bf_code_t bf;

    // check arguments
    if(argc < 2) {
        die("Need more arguments.");
    }

    // initialize bf object
    init_bf_object(&bf);

    // try to open file
    if((fp = fopen(argv[1], "rt")) == NULL) {
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

    // try to interpret it
    bfuck_execute(&bf);

    // exit
    exit(EXIT_SUCCESS);
}
