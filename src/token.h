#ifndef TOKEN_H
#define TOKEN_H

#include "string.h"

typedef enum {

	//Single char tokens
	TOK_LEFT_PAREN, TOK_RIGHT_PAREN, TOK_MINUS, TOK_PLUS, TOK_SLASH, TOK_STAR,
	TOK_DOT, TOK_COMMA, TOK_SEMICOLON, TOK_BANG, TOK_EQUAL, TOK_GREATER, TOK_LESS,


	// Tow char tokens
	TOK_BANG_EQUAL, TOK_EQUAL_EQUAL, TOK_GREATER_EQUAL, TOK_LESS_EQUAL,

	// literals
	TOK_IDENTIF, TOK_STRING, TOK_NUMBER,

	// Reserved keywords
	TOK_IF, TOK_ELSEIF, TOK_ELSE, TOK_RETURN, TOK_WHILE, TOK_AND, TOK_OR, TOK_TRUE, TOK_FALSE

} m_token_type;



typedef struct token_list m_token_list;
typedef struct token m_token;
typedef struct token_pos m_token_pos;

struct token_pos {
	int row;
	int col;
};

struct token_list {
	m_token* tlist;
	int length;
	int capacity;
};

struct token {
	m_token_type type;
	m_string *literal;
	m_token_pos pos;
};

m_token_list *m_token_list_new();
m_token_list *m_token_list_insert(m_token_list *tlist, m_token token);
m_token *m_token_list_get(m_token_list *tlist);
int m_token_list_length(m_token_list *tlist);
void m_token_list_destroy(m_token_list *tlist);
void m_token_list_print(m_token_list *tlist);
const char * m_token_get_token_name(int idx);


#endif // !TOKEN_H
