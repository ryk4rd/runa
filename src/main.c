#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

void parse_args(char **argv);

int main(int argc, char **argv) {

    if(argc == 1) {
        spawn_shell();
        return EXIT_SUCCESS;
    }

    parse_args(argv);

    return EXIT_SUCCESS;
}

void parse_args(char **argv) {
    printf("Parse some args!!!\n");
}


