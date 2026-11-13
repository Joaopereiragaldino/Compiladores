#ifndef _GEN_H_
#define _GEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct_compiler.h"
#include "symbols.h"

// Prototipos
void genAdd();
void genSub();
void genMult();
void genDiv();
void genNum(char num_string[MAX_TOKEN]);
void gen_preambule(void);
void gen_data_section(void);
void gen_preambule_code(void);
void gen_epilog_code(void);
void gen_label_name(char*);
void gen_label(char*);
void gen_cond_jump(char*);
void gen_incond_jump(char*);
void gen_bool(int);
void gen_bool_label_name(char*);
void gen_and(void);
void gen_or(void);
void gen_jal(char *label);
void gen_jr(void);

// Funcoes de Geracao de Codigo para Atribuicao
void genAssignInt(char *id, char *value);
void genAssignFloat(char *id, char *value);
void genAssignChar(char *id, char *value);
void genAssignString(char *id, char *value);
void gen_assign_var(char *id);
void gen_push_var(char *id);
void gen_push_char(char *id);


void gen_read(char*);
void gen_write(char*);
#endif //_GEN_H_