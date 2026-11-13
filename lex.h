#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct_compiler.h"

// Prototipos
void initLex(char*);
type_token *getToken();
type_token *keyWordFind(char*);
int peekChar(void);

#endif //_LEX_H_