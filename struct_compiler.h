/**
 * @file struct_compiler.h
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo para definicao de constantes e estruturas de dados do Sistema
 *        Compilador de C Simplificado
 * @version 0.3
 * @date 2021-12-09
 * 
 */

#ifndef _STRUCT_COMPILER_
#define _STRUCT_COMPILER_

// Definicao de constantes para todo o compilador
#define MAX_CHAR 32
#define MAX_TOKEN 32

#define true 1  //por conveniencia, para facilitar leitura de codigo
#define false 0 //por conveniencia, para facilitar leitura de codigo

// Definicao dos codigos dos tokens
#define ENDTOKEN '\0'
#define ERROR -1
#define NUM_INT 1
#define NUM_FLOAT 2
#define CHAR_VAL 3
#define STRING_VAL 4
#define PLUS '+'
#define MINUS '-'
#define MULT '*'
#define DIV '/'
#define OPEN_PAR '('
#define CLOSE_PAR ')'
#define SEMICOLON ';'
#define DOUBLE_QUOTES '"'
#define SINGLE_QUOTES '\''
#define ASSIGN '='
#define CLOSE_BRACE '}'
#define OPEN_BRACE '{'
#define COMMA ','

//KeyWords
#define ID 256
#define IF 257
#define ELSE 258
#define WHILE 260
#define DO 261
#define READ 262
#define WRITE 263
#define INT 264
#define FLOAT 265
#define STRING 266
#define CHAR 267
#define BEGIN 268
#define END 269

//Operadores 
#define EQUAL 270 // ==
#define NE 271  // !=
#define GE 272 // >=
#define LE 273 // <=
#define LG_AND 274
#define LG_OR 275
#define GT '>'
#define LT '<'
//#define ARROW
//#define CHAR



// Definicao da estrutura de dados 'token'
struct st_token {
    int tag;
    char lexema[MAX_CHAR];
    void *ptr_value;
};
typedef struct st_token type_token;

#endif //_STRUCT_COMPILER_
