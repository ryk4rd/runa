#include "token.h"
#include "string.h"
#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *token_names[] = {
    "TOK_LEFT_PAREN", "TOK_RIGHT_PAREN", "TOK_MINUS", "TOK_PLUS", "TOK_SLASH", "TOK_STAR",
    "TOK_DOT", "TOK_COMMA", "TOK_SEMICOLON", "TOK_BANG", "TOK_EQUAL", "TOK_GREATER", "TOK_LESS",
    "TOK_LEFT_BRACE", "TOK_RIGHT_BRACE", "TOK_COLON", "TOK_ARROW",


    // Tow char tokens
    "TOK_BANG_EQUAL", "TOK_EQUAL_EQUAL", "TOK_GREATER_EQUAL", "TOK_LESS_EQUAL",

    // literals
    "TOK_IDENTIF", "TOK_STRLIT", "TOK_NUMBER",

    // Reserved keywords
    "TOK_IF", "TOK_ELSEIF", "TOK_ELSE", "TOK_RETURN", "TOK_WHILE", "TOK_AND", "TOK_OR", "TOK_TRUE", "TOK_FALSE",
    "TOK_VAR", "TOK_INT", "TOK_FLOAT", "TOK_STRING"

};

struct reserved_words {
    char *word;
    m_token_type type;
};

// The complete list of reserved words.
const struct reserved_words reserved_words[] = {
    {"var", TOK_VAR},
    {"while", TOK_WHILE},
    {"return", TOK_RETURN},
    {"if", TOK_IF},
    {"else", TOK_ELSE},
    {"and", TOK_AND},
    {"or", TOK_OR},
    {"true", TOK_TRUE},
    {"false", TOK_FALSE},
    {"int", TOK_INT},
    {"float", TOK_INT},
    {"string", TOK_STRING},

};

m_string *_consume_number(char* s, int *i) {
    m_string *num = m_string_new();

    while(isdigit(s[*i]) || s[*i] == '.') {
        m_string_append_c(num, s[*i]);
        *i = *i+1;
    }

    *i = *i-1;

    return num;
}

m_string *_consume_word(char *s, int *i) {
    m_string *word = m_string_new();

    while(isalpha(s[*i]) || s[*i] == '_') {
        m_string_append_c(word, s[*i]);
        *i = *i+1;
    }

    *i = *i-1;

    return word;
}

m_string *_consume_strlit(char* s, int *i) {
    *i = *i+1; // skip the initial "
    m_string *strlit = m_string_new();

    while(s[*i] != '"') {
        m_string_append_c(strlit, s[*i]);
        *i = *i+1;
    }

    return strlit;
}

m_token_list *_tok_insert(m_token_list* tlist, m_token_type ttype, int i, m_string *lit, m_token_pos *pos) {

    m_token tok;

    tok.type = ttype;
    tok.literal = lit;
    tok.pos.col = pos->col;
    tok.pos.row = pos->row;
    m_token_list_insert(tlist, tok);

    return tlist;
}

char _tok_peek_next(m_string* s, int i) {
    if(i+1 > s->length) //return null term if trying to access outside buf.
        return '\0';
    return s->s[i+1];
}



int _tok_skip_comment(m_string *s, int i) {
    if (s == NULL || s->s == NULL || i >= s->length) {
        return i; // Return current position on invalid input
    }

    while (i < s->length && s->s[i] != '\n') {
        i++; // Skip comment content
    }
    if (i < s->length) {
        i++; // Move past newline
    }
    return i;
}

const char * m_token_get_token_name(int idx) {
    return token_names[idx];
}

m_token_list *m_token_list_new() {
    m_token_list *tlist = safe_malloc(sizeof(m_token_list));
    tlist->tlist = safe_malloc(sizeof(m_token)*24);
    tlist->capacity = 24;
    tlist->length = 0;

    return tlist;
}

m_token_list *m_token_list_insert(m_token_list *tlist, m_token token) {
    if(tlist->length+1 >= tlist->capacity) {
        tlist->capacity *= 2;
        tlist->tlist = safe_realloc(tlist->tlist, sizeof(m_token) * tlist->capacity);
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

    m_token_pos pos;
    pos.row = 1;
    pos.col = 0;
    for(int i = 0; i < s->length; i++) {
        m_token tok;
        switch (s->s[i]) {
            case ' ':
                continue;
            case '+':
                tlist = _tok_insert(tlist, TOK_PLUS,i, NULL, &pos);
                break;
            case '\n':
                pos.row++;
                pos.col = 0;
                break;
            case '-':
                if (_tok_peek_next(s, i) == '>') {
                    tlist = _tok_insert(tlist, TOK_ARROW,i, NULL, &pos);
                    i++;
                }else
                    tlist = _tok_insert(tlist, TOK_MINUS,i, NULL, &pos);
                break;
            case ':':
                tlist = _tok_insert(tlist, TOK_COLON,i, NULL, &pos);
                break;
            case '*':
                tlist = _tok_insert(tlist, TOK_STAR,i, NULL, &pos);
                break;

            case '(':
                tlist = _tok_insert(tlist, TOK_LEFT_PAREN,i, NULL, &pos);
                break;
            case ')':
                tlist = _tok_insert(tlist, TOK_RIGHT_PAREN,i, NULL, &pos);
                break;

            case '.':
                tlist = _tok_insert(tlist, TOK_DOT,i, NULL, &pos);
                break;

            case ',':
                tlist = _tok_insert(tlist, TOK_COMMA,i, NULL, &pos);
                break;

            case ';':
                tlist = _tok_insert(tlist, TOK_SEMICOLON,i, NULL, &pos);
                break;
            case '{':
                tlist = _tok_insert(tlist, TOK_LEFT_BRACE,i, NULL, &pos);
                break;
            case '}':
                tlist = _tok_insert(tlist, TOK_RIGHT_BRACE,i, NULL, &pos);
                break;
            case '/':
                if(_tok_peek_next(s, i) == '/') {
                    i = _tok_skip_comment(s, i);
                    i--; // avoid skipping next char
                    pos.row++;
                    pos.col = 1;
                } else {
                    tlist = _tok_insert(tlist, TOK_SLASH,i, NULL, &pos);
                }
                break;

            case '!':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_BANG_EQUAL,i, NULL, &pos);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_BANG,i, NULL, &pos);
                }
                break;

            case '=':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_EQUAL_EQUAL,i, NULL, &pos);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_EQUAL,i, NULL, &pos);
                }
                break;
            case '<':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_LESS_EQUAL,i, NULL, &pos);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_LESS, i, NULL, &pos);
                }
                break;
            case '>':
                if(_tok_peek_next(s, i) == '=') {
                    tlist = _tok_insert(tlist, TOK_GREATER_EQUAL,i, NULL, &pos);
                    i++;
                }
                else {
                    tlist = _tok_insert(tlist, TOK_GREATER, i, NULL, &pos);
                }
                break;
            case '"':
                { // ...Why i have to do this is beyond me.
                    m_string *strlit = _consume_strlit(s->s, &i);
                    tlist = _tok_insert(tlist, TOK_STRLIT, i, strlit, &pos);
                }
                break;
            default:
                if(isdigit(s->s[i])) {
                    m_string *num = _consume_number(s->s, &i);
                    tlist = _tok_insert(tlist, TOK_NUMBER, i, num, &pos);
                }else if (isalpha(s->s[i]) || s->s[i] == '_') {
                    m_string *word = _consume_word(s->s, &i);
                    int reserved_word_found = 0;
                    for (int i = 0; i < sizeof(reserved_words) / sizeof(reserved_words[0]); i++) {
                        if(strcmp(reserved_words[i].word, word->s) == 0) {
                            tlist = _tok_insert(tlist, reserved_words[i].type, i, word, &pos);
                            reserved_word_found = 1;
                        }
                    }
                    if(reserved_word_found == 0)
                        tlist = _tok_insert(tlist, TOK_IDENTIF, i, word, &pos);

                } else {
                    fprintf(stderr, "syntax error: unknown character '%c'\n", s->s[i]);

                }
                break;
        }
        pos.col++;
    }

    return tlist;
}
