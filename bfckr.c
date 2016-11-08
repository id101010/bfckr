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
#define MEMORY_SIZE 10000

// Memory initialized with zeros and its pointer
uint8_t memory[MEMORY_SIZE] = {0};
uint8_t *p = memory;

// Prototypes
void bfuck_parser(char *input);

// Error handler
void die(const char *message)
{
    if(errno) {
        perror(message);
    } else {
        printf("[:-(] Error: %s\n", message);
    }

    exit(EXIT_FAILURE);
}

// Parses and executes a brainfuck expression
void bfuck_parser(char *input)
{
    char curr;
    uint16_t loop;

    for(uint16_t i = 0; input[i] != 0; i++) {
        curr = input[i];

        switch(curr) {
        case '>':
            ++p;
            break;

        case '<':
            --p;
            break;

        case '+':
            ++*p;
            break;

        case '-':
            --*p;
            break;

        case '.':
            putchar(*p);
            break;

        case ',':
            *p = getchar();
            break;

        case '[':
            continue;
            break;

        case ']':
            if(*p) {
                loop = 1;
                while(loop) {
                    curr = input[--i];
                    if(curr == '[') {
                        loop--;
                    }
                    if(curr == ']') {
                        loop++;
                    }
                }
            }
            break;

        case ' ':
            break;

        default:
            printf("%c", *p);
            die("Invalid operant!");
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2) {
        die("Need more arguments.");
    }

    bfuck_parser(argv[1]);

    exit(EXIT_SUCCESS);
}
