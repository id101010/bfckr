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

// Parses and executes a brainfuck expression
void bfuck_parser(char *input)
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
    bfuck_parser(input);

    // exit
    exit(EXIT_SUCCESS);
}
