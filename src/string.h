#ifndef STRING_H
#define STRING_H

struct m_string {
    char *s;
    int capacity;
    int length;
};

typedef struct m_string m_string;

m_string* m_string_new();
m_string* m_string_append_str(m_string *str, char* c);
m_string* m_string_append_c(m_string *str,char c);
void m_string_destroy(m_string *str);

char * m_string_value(m_string *str);


#endif // !STRING_H
