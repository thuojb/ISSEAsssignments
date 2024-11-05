/*
 * parse.c
 * 
 * Recursive descent parser for ISSE Assignment 9
 *
 * Author: <your name here>
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "parse.h"
#include "tokenize.h"
#include "token.h"
#include "expr_tree.h"

// Recursive descent parsing functions
static ExprTree additive(CList tokens);
static ExprTree multiplicative(CList tokens);
static ExprTree exponential(CList tokens);
static ExprTree primary(CList tokens);

// Documented in .h file
ExprTree Parse(CList tokens)
{
    ExprTree tree = additive(tokens);

    if (TOK_next_type(tokens) != TOK_END) {
        printf("Syntax error on token %s\n", TT_to_str(TOK_next_type(tokens)));
        ET_free(tree);
        return NULL;
    }

    return tree;
}

static ExprTree additive(CList tokens)
{
    ExprTree lhs = multiplicative(tokens);

    while (TOK_next_type(tokens) == TOK_PLUS || TOK_next_type(tokens) == TOK_MINUS) {
        TokenType op = TOK_next_type(tokens);
        TOK_consume(tokens);
        ExprTree rhs = multiplicative(tokens);
        if (op == TOK_PLUS) {
            lhs = ET_new_add(lhs, rhs);
        } else {
            lhs = ET_new_sub(lhs, rhs);
        }
    }

    return lhs;
}

static ExprTree multiplicative(CList tokens)
{
    ExprTree lhs = exponential(tokens);

    while (TOK_next_type(tokens) == TOK_MULTIPLY || TOK_next_type(tokens) == TOK_DIVIDE) {
        TokenType op = TOK_next_type(tokens);
        TOK_consume(tokens);
        ExprTree rhs = exponential(tokens);
        if (op == TOK_MULTIPLY) {
            lhs = ET_new_mul(lhs, rhs);
        } else {
            lhs = ET_new_div(lhs, rhs);
        }
    }

    return lhs;
}

static ExprTree exponential(CList tokens)
{
    ExprTree lhs = primary(tokens);

    if (TOK_next_type(tokens) == TOK_POWER) {
        TOK_consume(tokens);
        ExprTree rhs = exponential(tokens);
        lhs = ET_new_pow(lhs, rhs);
    }

    return lhs;
}

static ExprTree primary(CList tokens)
{
    if (TOK_next_type(tokens) == TOK_VALUE) {
        Token tok = TOK_next(tokens);
        TOK_consume(tokens);
        return ET_new_const(tok.value);
    } else if (TOK_next_type(tokens) == TOK_OPEN_PAREN) {
        TOK_consume(tokens);
        ExprTree expr = additive(tokens);
        if (TOK_next_type(tokens) != TOK_CLOSE_PAREN) {
            printf("Syntax error on token %s\n", TT_to_str(TOK_next_type(tokens)));
            ET_free(expr);
            return NULL;
        }
        TOK_consume(tokens);
        return expr;
    } else if (TOK_next_type(tokens) == TOK_MINUS) {
        TOK_consume(tokens);
        ExprTree expr = primary(tokens);
        return ET_new_neg(expr);
    } else {
        printf("Syntax error on token %s\n", TT_to_str(TOK_next_type(tokens)));
        return NULL;
    }
}