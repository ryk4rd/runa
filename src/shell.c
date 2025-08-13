#include "shell.h"
#include "string.h"
#include "token.h"
#include <stdio.h>

#include "tokenize.h"

void spawn_shell() {
    printf("welcome to the mylang interactive shell. V 0.1\n");
    for (;;) { // Shell main loop
        printf(">>> ");

        char c;
        m_string *s = m_string_new();

        while ((c = fgetc(stdin)) != '\n' && c != EOF) {
            s = m_string_append_c(s, c);
        }

        m_token_list *tlist = m_tokenize_scan(s);
        m_token_list_print(tlist);

        m_token_list_destroy(tlist);
        m_string_destroy(s);

        if (c == EOF) {
            printf("Bye!\n");
            break;
        }
    }
}

