#include "shell.h"
#include "string.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#define STR_ALLOC_BUF 24


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

void spawn_shell() {
	printf("welcome to the mylang interactive shell. V 0.1\n");
	for (;;) { // Shell main loop
		printf(">>> ");

		char c;
		int curr_capacity = STR_ALLOC_BUF;
		m_string *s = m_string_new();

		while ((c = fgetc(stdin)) != '\n' && c != EOF) {
			s = m_string_append_c(s, c);
		}

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
					tlist = _tok_insert(tlist, TOK_SLASH,i);
				break;
			}			
		}

		m_token_list_print(tlist);
		m_token_list_destroy(tlist);


		m_string_destroy(s);


		if (c == EOF) {
			printf("Bye!\n");
			break;
		}
	}
}

