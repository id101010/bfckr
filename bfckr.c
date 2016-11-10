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

// Amount of memory on the band tape
#define MEMORY_SIZE     10000
#define MAX_INPUT_SIZE  10000
#define clear()         printf("\033[H\033[J")

// Memory initialized with zeros and its pointer
char memory[MEMORY_SIZE] = {0};
char *p = memory;
int memcnt = 0;

// Prototypes
void bfuck_parser(char *input);

// Error handler
void die(const char *message)
{
    if(errno) {
        perror(message);
    } else {
        printf("[:(] Error: %s\n", message);
    }

    exit(EXIT_FAILURE);
}

// Prints the bf source at the current location
void print_sourceviewer()
{
    printf("Source viewer:"
           "-----------------------------------------------------------"
           "_____________________________>+++++++++++++++[<+>>>>>>>>+++"
           "                             ^                             "
           "                             ip=0                          "
           "-----------------------------------------------------------"); // just to get the idea ...
}

// Prints memory information at the current memory location
void print_memoryviewer()
{
    int pointerlocation = (p - memory)*sizeof(*memory); // find the arrayindex at which the pointer is pointing

    printf("Memory viewer:                                             "
           "-----------------------------------------------------------"
           "000 000 000 000 000 000 001 001 000 000 000 000 000 000 000"
           "                             ^                             "
           "                            mp=1                           "
           "249 250 251 252 253 254 000 001 002 003 004 005 006 007 008"
           "-----------------------------------------------------------"); // just to get the idea ...
}

// Pauses the program flow and prints information
void bfuck_debugger(char *bf_source_input, int instructionpointer)
{
    clear(); // clear terminal

    printf("[s]: single step [c]: continue");

    print_sourceviewer();
    print_memoryviewer();

    switch(getchar()) {
    case 's':
        // single step
        break;
    case 'c':
        // continue
        break;
    }

}

// Parses and executes a brainfuck expression
void bfuck_execute(char *input)
{
    int loop = 0;

    for(size_t i = 0; input[i] != 0; i++) { // where i is the instruction pointer
        switch(input[i]) {
        case '>':
            if(memcnt >= MEMORY_SIZE) { // prevent overrun
                p = &memory[0];
                memcnt = 0;
            } else {
                ++p; // increment data pointer
                ++memcnt;
            }
            break;

        case '<':
            if(memcnt < 0) { // prevent underun
                p = &memory[MEMORY_SIZE - 1];
                memcnt = MEMORY_SIZE - 1;
            } else {
                --p; // decrement data pointer
                --memcnt;
            }
            break;

        case '+':
            ++(*p); // increment byte at data pointer
            break;

        case '-':
            --(*p); // decrement byte at data pointer
            break;

        case '.':
            putchar(*p); // output the byte at data pointer
            break;

        case ',':
            *p = getchar(); // accept one byte of input and store it at data pointer
            break;

        case '[':
            if(*p == 0) { // if the byte at the data pointer is zero
                // jump forward to the command after the next ]
                loop = 1;
                while(loop > 0) { // count nested loops and make sure to get the matching ]
                    i++;
                    if(input[i] == '[') {
                        loop++;
                    }
                    if(input[i] == ']') {
                        loop--;
                    }
                }
            } else {
                continue; // increment instruction pointer
            }
            break;

        case ']':
            if(*p != 0) { // if the byte at the data pointer is nonzero
                // jump back to the command after the matching [
                loop = 1;
                while(loop > 0) { // count nested loops and make sure to get the matching [
                    i--;
                    if(input[i] == '[') {
                        loop--;
                    }
                    if(input[i] == ']') {
                        loop++;
                    }
                }

            } else {
                continue; // increment instruction pointer
            }
            break;

        case '#':
            // Breakpoint reached, start debugger ...
            break;

        default:
            // Do nothing
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    FILE *fp;
    int i = 0;
    int c;
    char input[MAX_INPUT_SIZE];

    // check arguments
    if(argc < 2) {
        die("Need more arguments.");
    }

    // try to open file
    if((fp = fopen(argv[1], "rt")) == NULL) {
        die("Couldn't open file.");
    }

    // read the file and store it in the input buffer
    while((c = getc(fp)) != EOF) {
        input[i++] = c;
    }

    // close file after reading
    fclose(fp);

    // try to interpret it
    bfuck_execute(input);

    // exit
    exit(EXIT_SUCCESS);
}
