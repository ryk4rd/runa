#include "string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


m_string *_m_string_realloc(m_string *str) {

	char *tmp = realloc(str->s, sizeof(char)*str->capacity);
	if(tmp == NULL) {
		perror("could not reallocate string buffer");
		free(str);
		exit(EXIT_FAILURE);
	}
	str->s = tmp;
	return str;	
}

m_string* m_string_new() {
	m_string *str = malloc(sizeof(m_string));
	str->capacity = 24;
	str->length = 0;

	str->s = malloc(sizeof(char)*str->capacity);

	return str;
}
m_string* m_string_append_c(m_string *str, char c) {
	if(str->length+1 >= str->capacity) {
		str->capacity *= 2;
		str = _m_string_realloc(str);
	}

	str->s[str->length] = c;
	str->length++;
	str->s[str->length] = '\0';

	return str;
}
m_string* m_string_append_str(m_string *str, char* c) {
	if(str->capacity + strlen(c) + 1 >= str->capacity) {
		str->capacity = str->capacity + strlen(c);
		str = _m_string_realloc(str);
	}

	strcat(str->s, c);
	return str;
}

void m_string_destroy(m_string *str) {
	free(str->s);
	free(str);
}

char * m_string_value(m_string *str) {
	return str->s;
}

