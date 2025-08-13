//
// Created by marcus on 8/11/25.
//

#include "tokenize.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

struct reserved_words {
    const char *word;
    m_token_type type;
};

struct reserved_annotations {
    const char *word;
    m_token_type type;
};

static const struct reserved_words reserved_words[] = {
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
    {"float", TOK_FLOAT},
    {"string", TOK_STRING},
    {"from", TOK_FROM},
    {"use", TOK_USE},
    {"module", TOK_MODULE},
    {"def", TOK_DEF},
};

static const struct reserved_annotations reserved_annotations[] = {
    {"core", TOK_ANNOTATION_CORE},
};

/* Consume functions:
 * - take the source buffer and a start index
 * - return a newly-allocated m_string* and set *end to the index ONE PAST
 *   the last character consumed (so end - start == number of chars consumed)
 *
 * This makes updating pos.col and i straightforward.
 */
static m_string *m_tokenize_consume_number(const char *s, int start, int *end) {
    m_string *num = m_string_new();
    int i = start;
    bool seen_dot = false;

    if (!isdigit((unsigned char)s[i])) {
        fprintf(stderr, "internal error: consume_number called at non-digit\n");
        exit(EXIT_FAILURE);
    }

    while (s[i] != '\0' && (isdigit((unsigned char)s[i]) || s[i] == '.')) {
        if (s[i] == '.') {
            if (seen_dot) {
                fprintf(stderr, "syntax error: malformed number (multiple dots) at index %d\n", start);
                exit(EXIT_FAILURE);
            }
            seen_dot = true;
        }
        m_string_append_c(num, s[i]);
        i++;
    }

    *end = i;
    return num;
}

static m_string *m_tokenize_consume_word(const char *s, int start, int *end) {
    m_string *word = m_string_new();
    int i = start;

    if (!(isalpha((unsigned char)s[i]) || s[i] == '_')) {
        fprintf(stderr, "internal error: consume_word called at non-alpha/_\n");
        exit(EXIT_FAILURE);
    }

    // first char already validated
    m_string_append_c(word, s[i]);
    i++;

    // subsequent characters: allow alnum and underscore
    while (s[i] != '\0' && (isalnum((unsigned char)s[i]) || s[i] == '_')) {
        m_string_append_c(word, s[i]);
        i++;
    }

    *end = i;
    return word;
}

static m_string *m_tokenize_consume_strlit(const char *s, int start, int *end) {
    // s[start] must be '"'
    if (s[start] != '"') {
        fprintf(stderr, "internal error: consume_strlit called at non-quote\n");
        exit(EXIT_FAILURE);
    }

    m_string *strlit = m_string_new();
    int i = start + 1;

    while (s[i] != '\0') {
        if (s[i] == '\\') {
            // handle simple escapes: \n, \t, \r, \", \\, others - take literal char
            if (s[i + 1] == '\0') {
                fprintf(stderr, "unterminated escape in string literal\n");
                exit(EXIT_FAILURE);
            }
            i++;
            switch (s[i]) {
                case 'n': m_string_append_c(strlit, '\n'); break;
                case 't': m_string_append_c(strlit, '\t'); break;
                case 'r': m_string_append_c(strlit, '\r'); break;
                case '"': m_string_append_c(strlit, '"'); break;
                case '\\': m_string_append_c(strlit, '\\'); break;
                default:
                    // unknown escape: append the char itself (e.g. \x -> x)
                    m_string_append_c(strlit, s[i]);
                    break;
            }
            i++;
            continue;
        }

        if (s[i] == '"') {
            i++; // consume closing quote
            *end = i;
            return strlit;
        }

        m_string_append_c(strlit, s[i]);
        i++;
    }

    fprintf(stderr, "unterminated string literal\n");
    exit(EXIT_FAILURE);
}

static int m_tokenize_skip_comment(m_string *s, int start, int *end) {
    // start points to the first '/' of '//'
    int i = start;
    while (i < s->length && s->s[i] != '\n') i++;
    // position one past newline (or s->length if EOF)
    *end = (i < s->length) ? i + 1 : i;
    return 0;
}

static m_token_list *_m_tokenize_consume_annotation(
    m_token_list *tlist, m_string *s, int start, m_token_pos *pos, int *out_i
) {
    // start points at the first char of the word (i.e. after '@')
    int end;
    m_string *word = m_tokenize_consume_word(s->s, start, &end);

    bool reserved_found = false;
    for (size_t ann_idx = 0; ann_idx < ARRAY_LEN(reserved_annotations); ann_idx++) {
        if (strcmp(reserved_annotations[ann_idx].word, word->s) == 0) {
            tlist = m_tokenize_token_insert(tlist, reserved_annotations[ann_idx].type, word, pos);
            reserved_found = true;
            break;
        }
    }
    if (!reserved_found) {
        fprintf(stderr, "Unknown annotation word: %s\n", word->s);
        exit(EXIT_FAILURE);
    }

    // update caller's index (one past last consumed char)
    *out_i = end;
    return tlist;
}

m_token_list *m_tokenize_scan(m_string *s) {
    m_token_list *tlist = m_token_list_new();
    m_token_pos pos = { .row = 1, .col = 0 };

    for (int i = 0; i < s->length; /* updated inside */) {
        char c = s->s[i];

        // whitespace
        if (c == ' ' || c == '\t') {
            i++;
            pos.col++;
            continue;
        }

        // newline
        if (c == '\n') {
            i++;
            pos.row++;
            pos.col = 0;
            continue;
        }

        // single or multi-char tokens
        if (c == '+')        { tlist = m_tokenize_token_insert(tlist, TOK_PLUS, NULL, &pos); i++; pos.col++; continue; }
        if (c == ',')        { tlist = m_tokenize_token_insert(tlist, TOK_COMMA, NULL, &pos); i++; pos.col++; continue; }
        if (c == ';')        { tlist = m_tokenize_token_insert(tlist, TOK_SEMICOLON, NULL, &pos); i++; pos.col++; continue; }
        if (c == ':')        { tlist = m_tokenize_token_insert(tlist, TOK_COLON, NULL, &pos); i++; pos.col++; continue; }
        if (c == '*')        { tlist = m_tokenize_token_insert(tlist, TOK_STAR, NULL, &pos); i++; pos.col++; continue; }
        if (c == '(')        { tlist = m_tokenize_token_insert(tlist, TOK_LEFT_PAREN, NULL, &pos); i++; pos.col++; continue; }
        if (c == ')')        { tlist = m_tokenize_token_insert(tlist, TOK_RIGHT_PAREN, NULL, &pos); i++; pos.col++; continue; }
        if (c == '.')        { tlist = m_tokenize_token_insert(tlist, TOK_DOT, NULL, &pos); i++; pos.col++; continue; }
        if (c == '{')        { tlist = m_tokenize_token_insert(tlist, TOK_LEFT_BRACE, NULL, &pos); i++; pos.col++; continue; }
        if (c == '}')        { tlist = m_tokenize_token_insert(tlist, TOK_RIGHT_BRACE, NULL, &pos); i++; pos.col++; continue; }

        // annotation: @ident
        if (c == '@') {
            char next = m_tokenize_peek_next(s, i);
            if (isalpha((unsigned char)next) || next == '_') {
                // consume the '@' then the word
                int after_word;
                tlist = _m_tokenize_consume_annotation(tlist, s, i + 1, &pos, &after_word);
                // bytes consumed: after_word - i
                pos.col += (after_word - i);
                i = after_word;
                continue;
            } else {
                fprintf(stderr, "syntax error: expecting identifier after annotation at row %d col %d\n", pos.row, pos.col);
                exit(EXIT_FAILURE);
            }
        }

        // comments or slash
        if (c == '/') {
            if (m_tokenize_peek_next(s, i) == '/') {
                int after;
                m_tokenize_skip_comment(s, i, &after);
                // if skipped a newline, we set row++ and col=0; otherwise advance col
                if (after > i && after <= s->length && s->s[after - 1] == '\n') {
                    pos.row++;
                    pos.col = 0;
                } else {
                    pos.col += (after - i);
                }
                i = after;
                continue;
            } else {
                tlist = m_tokenize_token_insert(tlist, TOK_SLASH, NULL, &pos);
                i++; pos.col++;
                continue;
            }
        }

        // two-char operators
        if (c == '-' && m_tokenize_peek_next(s, i) == '>') {
            tlist = m_tokenize_token_insert(tlist, TOK_ARROW, NULL, &pos);
            i += 2; pos.col += 2;
            continue;
        }
        if (c == '!' && m_tokenize_peek_next(s, i) == '=') {
            tlist = m_tokenize_token_insert(tlist, TOK_BANG_EQUAL, NULL, &pos);
            i += 2; pos.col += 2;
            continue;
        }
        if (c == '=' && m_tokenize_peek_next(s, i) == '=') {
            tlist = m_tokenize_token_insert(tlist, TOK_EQUAL_EQUAL, NULL, &pos);
            i += 2; pos.col += 2;
            continue;
        }
        if (c == '<' && m_tokenize_peek_next(s, i) == '=') {
            tlist = m_tokenize_token_insert(tlist, TOK_LESS_EQUAL, NULL, &pos);
            i += 2; pos.col += 2;
            continue;
        }
        if (c == '>' && m_tokenize_peek_next(s, i) == '=') {
            tlist = m_tokenize_token_insert(tlist, TOK_GREATER_EQUAL, NULL, &pos);
            i += 2; pos.col += 2;
            continue;
        }

        // single-char variants of above or other single-char tokens
        if (c == '-') { tlist = m_tokenize_token_insert(tlist, TOK_MINUS, NULL, &pos); i++; pos.col++; continue; }
        if (c == '!') { tlist = m_tokenize_token_insert(tlist, TOK_BANG, NULL, &pos); i++; pos.col++; continue; }
        if (c == '=') { tlist = m_tokenize_token_insert(tlist, TOK_EQUAL, NULL, &pos); i++; pos.col++; continue; }
        if (c == '<') { tlist = m_tokenize_token_insert(tlist, TOK_LESS, NULL, &pos); i++; pos.col++; continue; }
        if (c == '>') { tlist = m_tokenize_token_insert(tlist, TOK_GREATER, NULL, &pos); i++; pos.col++; continue; }

        // string literal
        if (c == '"') {
            int end;
            m_string *strlit = m_tokenize_consume_strlit(s->s, i, &end);
            tlist = m_tokenize_token_insert(tlist, TOK_STRLIT, strlit, &pos);
            pos.col += (end - i);
            i = end;
            continue;
        }

        // number literal (starts with digit)
        if (isdigit((unsigned char)c)) {
            int end;
            m_string *num = m_tokenize_consume_number(s->s, i, &end);
            tlist = m_tokenize_token_insert(tlist, TOK_NUMBER, num, &pos);
            pos.col += (end - i);
            i = end;
            continue;
        }

        // identifier / reserved word (starts with alpha or _)
        if (isalpha((unsigned char)c) || c == '_') {
            int end;
            m_string *word = m_tokenize_consume_word(s->s, i, &end);
            bool reserved_found = false;
            for (size_t rw_idx = 0; rw_idx < ARRAY_LEN(reserved_words); rw_idx++) {
                if (strcmp(reserved_words[rw_idx].word, word->s) == 0) {
                    tlist = m_tokenize_token_insert(tlist, reserved_words[rw_idx].type, word, &pos);
                    reserved_found = true;
                    break;
                }
            }
            if (!reserved_found) {
                tlist = m_tokenize_token_insert(tlist, TOK_IDENTIF, word, &pos);
            }
            pos.col += (end - i);
            i = end;
            continue;
        }

        // unknown character
        fprintf(stderr, "syntax error: unknown character '%c' at row %d col %d\n", c, pos.row, pos.col);
        exit(EXIT_FAILURE);
    }

    return tlist;
}

m_token_list *m_tokenize_token_insert(m_token_list *tlist, m_token_type ttype, m_string *lit, m_token_pos *pos) {
    m_token tok;
    tok.type = ttype;
    tok.literal = lit;
    tok.pos.col = pos->col;
    tok.pos.row = pos->row;
    m_token_list_insert(tlist, tok);
    return tlist;
}

char m_tokenize_peek_next(m_string *s, int i) {
    if (i + 1 >= s->length)
        return '\0';
    return s->s[i + 1];
}
