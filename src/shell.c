#include "shell.h"
#include "string.h"
#include "token.h"
#define STR_ALLOC_BUF 24

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
					tok.type = TOK_PLUS;
					tok.literal = m_string_new();
					m_string_append_str(tok.literal, "hej");
					tok.pos.col = 1;
					tok.pos.row = 1;
					m_token_list_insert(tlist, tok);
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

