#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "string.h"
#include "token.h"

void parse_args(int argc, char **argv);

int main(int argc, char **argv) {

    if(argc == 1) {
        spawn_shell();
        return EXIT_SUCCESS;
    }

    parse_args(argc, argv);

    return EXIT_SUCCESS;
}

void parse_args(int argc, char **argv) {

    for(int i = 1; i < argc; i++) {

        FILE *fp = fopen(argv[i], "r");

        char c;
        m_string *str = m_string_new();

        while((c = fgetc(fp)) != EOF) {
            str = m_string_append_c(str, c);
        }

        m_token_list *tlist = m_token_scan(str);

        m_token_list_print(tlist);

        m_token_list_destroy(tlist);

        m_string_destroy(str);
        fclose(fp);

    }
}


