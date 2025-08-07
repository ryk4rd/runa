#include "string.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

m_string* m_string_new() {
    m_string *str = safe_malloc(sizeof(m_string));
    str->capacity = 24;
    str->length = 0;

    str->s = safe_malloc(sizeof(char)*str->capacity);

    return str;
}
m_string* m_string_append_c(m_string *str, char c) {
    if(str->length+1 >= str->capacity) {
        str->capacity *= 2;
        str->s = safe_realloc(str->s, sizeof(char)*str->capacity);
    }

    str->s[str->length] = c;
    str->length++;
    str->s[str->length] = '\0';

    return str;
}
m_string* m_string_append_str(m_string *str, char* c) {
    if(str->capacity + strlen(c) + 1 >= str->capacity) {
        str->capacity = str->capacity + strlen(c);
        str->s = safe_realloc(str->s, sizeof(char)*str->capacity);
    }

    strcat(str->s, c);
    return str;
}

void m_string_destroy(m_string *str) {
    if(str->s != NULL)
        free(str->s);

    if(str != NULL)
        free(str);
}

char * m_string_value(m_string *str) {
    return str->s;
}

