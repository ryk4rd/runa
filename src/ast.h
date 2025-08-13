//
// Created by marcus on 8/11/25.
//

#ifndef RUNA_AST_H
#define RUNA_AST_H
#include "token.h"

typedef enum {
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_UNARY
} m_expr_type;


typedef struct m_expr m_expr;
typedef struct m_binary_node m_binary_node;
typedef struct m_grouping m_grouping;
typedef struct m_unary m_unary;


struct m_binary_node {
    m_expr *left;
    m_token operator;
    m_expr *right;
};

struct m_grouping {
    m_expr *expr;
};
struct m_unary {
    m_token operator;
    m_expr *right;
};

struct m_expr {
    m_expr_type type;
    union {
        m_binary_node *bin;
        m_grouping *grouping;
        m_unary *unary;
    };
};


#endif //RUNA_AST_H
