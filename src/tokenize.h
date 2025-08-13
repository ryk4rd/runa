#ifndef RUNA_TOKENIZE_H
#define RUNA_TOKENIZE_H
#include "token.h"

m_token_list *m_tokenize_scan(m_string *s);

char m_tokenize_peek_next(m_string *s, int i);

m_token_list *m_tokenize_token_insert(m_token_list *tlist, m_token_type ttype, m_string *lit, m_token_pos *pos);

#endif // RUNA_TOKENIZE_H
