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
		printf("\t\tliteral:%s\n", tlist->tlist[i].literal->s);
		printf("\t\tpos:(%d,%d)\n", tlist->tlist[i].pos.row, tlist->tlist[i].pos.col);
		printf("\t},\n");
	}

	printf("}\n");
}
