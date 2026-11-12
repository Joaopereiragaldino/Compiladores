/**
 * @file synt.h
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo do analisado sintatico
 * @version 0.4
 * @date 2022-02-04
 * 
 */

#ifndef _SYNT_H_
#define _SYNT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct_compiler.h"
#include "lex.h"
#include "gen.h"
#include "symbols.h"

/*
 * Gramatica:
 *      program      -> declarations statements
 *      declarations -> declaration declarations | [vazio]
 *      declaration  -> (int|float|char|string) id ;
 *      statements   -> statement statements | [vazio]
 *      statement    -> read id ; | write id ;
 *                    | id = valor ;   (valor conforme o tipo de id:
 *                                      int: expressao | float: numero
 *                                      char: 'x' | string: "texto")
 *                    | if ( B ) bloco [ else bloco ]
 *                    | while ( B ) bloco
 *      bloco        -> ( begin | { ) statements ( end | } )
 *      B            -> B_term { || B_term }
 *      B_term       -> B_fact { && B_fact }
 *      B_fact       -> E op_rel E        (op_rel: == != < > <= >=)
 *      E -> T E' ; E' -> + T E' | - T E' | [vazio]
 *      T -> F T' ; T' -> * F T' | / F T' | [vazio]
 *      F -> ( E ) | num_int | 'c' | id   (id: int ou char)
 */


// Prototipos
int match(int token_tag);
int program (void);
void func_codes(void);
int declarations(void);
int statements (void);
int declaration(void);
int statement(void);

int B(void);
int B_term(void);
int B_fact(void);
int boolOperator(int *operator);

int E(void);
int ER(void);
int T(void);
int TR(void);
int F(void);

int main(int argc, char *argv[]);

#endif //_SYNT_H_