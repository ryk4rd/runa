#include "token.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

const char *token_names[] = {
    "TOK_LEFT_PAREN", "TOK_RIGHT_PAREN", "TOK_MINUS", "TOK_PLUS", "TOK_SLASH", "TOK_STAR",
    "TOK_DOT", "TOK_COMMA", "TOK_SEMICOLON", "TOK_BANG", "TOK_EQUAL", "TOK_GREATER", "TOK_LESS",


    // Tow char tokens
    "TOK_BANG_EQUAL", "TOK_EQUAL_EQUAL", "TOK_GREATER_EQUAL", "TOK_LESS_EQUAL",

    // literals
    "TOK_IDENTIF", "TOK_STRING", "TOK_NUMBER",

    // Reserved keywords
    "TOK_IF", "TOK_ELSEIF", "TOK_ELSE", "TOK_RETURN", "TOK_WHILE", "TOK_AND", "TOK_OR", "TOK_TRUE", "TOK_FALSE"

};

m_token_list *_tok_insert(m_token_list* tlist, m_token_type ttype, int i) {

    m_token tok;
    m_token_pos pos;

    tok.type = ttype;
    tok.literal = NULL;
    tok.pos.col = i+1;
    tok.pos.row = 1;
    m_token_list_insert(tlist, tok);

    return tlist;
}

char _tok_peek_next(m_string* s, int i) {
    if(i+1 > s->length) //return null term if trying to access outside buf.
        return '\0';
    return s->s[i+1];
}

const char * m_token_get_token_name(int idx) {
    return token_names[idx];
}

m_token_list *m_token_list_new() {
    m_token_list *tlist = malloc(sizeof(m_token_list));
    tlist->tlist = malloc(sizeof(m_token)*24);
    tlist->capacity = 24;
    tlist->length = 0;

    return tlist;
}

m_token_list *m_token_list_insert(m_token_list *tlist, m_token token) {
    if(tlist->length+1 >= tlist->capacity) {
        tlist->capacity *= 2;
        m_token *tmp = realloc(tlist->tlist, tlist->capacity);
        if (tmp == NULL) {
            perror("could not reallocate tlist buf");
            free(tlist->tlist);
            free(tlist);
            exit(EXIT_FAILURE);

        }
        tlist->tlist = tmp;
    }

    tlist->tlist[tlist->length] = token;
    tlist->length++;

    return tlist;
}
m_token *m_token_list_get(m_token_list *tlist) {
    return tlist->tlist;
}
int m_token_list_length(m_token_list *tlist) {
    return tlist->length;
}

void m_token_list_destroy(m_token_list *tlist) {
    for(int i = 0; i < tlist->length; i++) {
        if(tlist->tlist[i].literal != NULL) {
            m_string_destroy(tlist->tlist[i].literal);
        }
    }

    free(tlist->tlist);
    free(tlist);
}


void m_token_list_print(m_token_list *tlist) {
    printf("{\n");

    for(int i = 0; i < tlist->length; i++) {
        printf("\t{\n");
        printf("\t\ttype:%s\n", m_token_get_token_name(tlist->tlist[i].type));
        if(tlist->tlist[i].literal == NULL)
            printf("\t\tliteral:NULL\n");
        else
            printf("\t\tliteral:%s\n", tlist->tlist[i].literal->s);
        printf("\t\tpos:(%d,%d)\n", tlist->tlist[i].pos.row, tlist->tlist[i].pos.col);
        printf("\t},\n");
    }

    printf("}\n");
}

m_token_list *m_token_scan(m_string *s) {
    m_token_list *tlist = m_token_list_new();

    for(int i = 0; i < s->length; i++) {
        m_token tok;
        m_token_pos pos;
        switch (s->s[i]) {
            case '+':
                tlist = _tok_insert(tlist, TOK_PLUS,i);
                break;

            case '-':
                tlist = _tok_insert(tlist, TOK_MINUS,i);
                break;

            case '*':
                tlist = _tok_insert(tlist, TOK_STAR,i);
                break;

            case '/':
                if(_tok_peek_next(s, i) == '/') {
                    for(;;) {
                        i++;
                        if(_tok_peek_next(s, i) != '\0' || _tok_peek_next(s, '\n')) {
                            i++;
                            break;
                        }

                    }
                    i++;
                } else {
                    tlist = _tok_insert(tlist, TOK_SLASH,i);
                }
                break;

            case '!':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_BANG_EQUAL,i);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_BANG,i);
                }
                break;

            case '=':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_EQUAL_EQUAL,i);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_EQUAL,i);
                }
                break;
            case '<':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_LESS_EQUAL,i);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_LESS, i);
                }
                break;
            case '>':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_GREATER_EQUAL,i);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_GREATER, i);
                }
                break;
        }			
    }

    return tlist;
}
