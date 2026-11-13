#include "symbols.h"

//extern int nstringconsts;
//int sympos = 0;
//int nstringconsts = 0;
//int symfuncspos = 0; //Sera usado posteriormente
//int stack_pos = 0;

//Variaveis globais
type_symbol_table_variables global_symbol_table_variables;
type_symbol_table_string symbol_table_string;
type_symbol_table_func symbol_table_func;
 

/**
 * @brief Busca pelo simbolo na tabela de variaveis
 * 
 * @param char*
 * @param type_symbol_table_variables*
 * @return type_symbol_table_entry* 
 */
type_symbol_table_entry *sym_find(char *s, type_symbol_table_variables *stv) {
	int i;
	type_symbol_table_entry *symbol = NULL;
	for (i = 0; i < stv->n_variables; i++) {
		if (strcmp(stv->variable[i].name, s) == 0) {
			symbol = &(stv->variable[i]);
		}
	}
	return symbol;
}

/**
 * @brief Insere um novo simbolo na tabela de variaveis
 * 
 * @param name 
 * @param type 
 * @param addr 
 * @return type_symbol* 
 */
 type_symbol_table_entry *sym_declare(char *name, int type, int addr, void *ptr_value,type_symbol_table_variables *stv) {
	if (stv->n_variables < MAX_SYMBOLS) { //Verifica se eh possivel declarar mais variavel
		int n_var;
        n_var = stv->n_variables;
        stv->variable[n_var].type = type;
        stv->variable[n_var].addr = addr;
        stv->variable[n_var].ptr_value = ptr_value;
        strcpy(stv->variable[n_var].name, name);
        stv->n_variables++;
        //n_var = stv->n_variables;
        return &(stv->variable[n_var]);
	} else {
        printf("[ERRO] Limite de declaracao (quantidade) de variaveis atingido.\n");
        return NULL;
    }
}

/**
 * @brief Busca pelo simbolo na tabela de STRINGS
 * 
 * @param char*
 * @return type_symbol_table_string_entry* 
 */
type_symbol_table_string_entry *sym_string_find(char *s) {
    int i;
    type_symbol_table_string_entry *str = NULL;
    for (i = 0; i < symbol_table_string.n_strings; i++ ) {
        if ( strcmp(symbol_table_string.string[i].value, s) ) {
            str = &(symbol_table_string.string[i]);
        }
    }
    return str;
}

/**
 * @brief Insere um novo simbolo na tabela de STRINGS
 * 
 * @param char*
 * @return type_symbol_table_string_entry* 
 */
type_symbol_table_string_entry *sym_string_declare(char *s) {
    static int str_label_count = 0;

    if ( symbol_table_string.n_strings < MAX_STRINGS ) {
        int n_str;
        char str_name[MAX_TOKSZ];
        sprintf(str_name, "str%d", str_label_count);
        n_str = symbol_table_string.n_strings;

        strcpy(symbol_table_string.string[n_str].value, s);
        strcpy(symbol_table_string.string[n_str].name, str_name);
        str_label_count++;
        symbol_table_string.n_strings++;
        return &(symbol_table_string.string[n_str]) ;
    } else {
        printf("[ERRO] Numero maximo de strings declaradas alcancado!\n");
        return NULL;
    }
}


/**
 * @brief Busca pelo simbolo na tabela de simbolos de funcoes
 * 
 * @return type_symbol_func_entry
 */
type_symbol_table_func_entry *sym_func_find(char *f_name){
    int i;
	type_symbol_table_func_entry *func;
    func = NULL;

	for (i = 0; i < symbol_table_func.n_funcs; i++) {
		if (strcmp(symbol_table_func.funcs[i].name, f_name) == 0) {
			func = &symbol_table_func.funcs[i];
		}
	}
	return func;
}


/**
 * @brief Insere um novo simbolo na tabela de funcoes.
 * 
 * @param name nome da funcao
 * @param type tipo da funcao
 * @param params lista de parametros
 * @param nparams quantidade de parametros
 * @return type_symbol_function_entry* posicao de memoria da funcao inserida 
 */
type_symbol_table_func_entry *sym_func_declare(char *name, int type, type_symbol_table_entry params[MAX_PARAMS], int n_params) {
    int i, pos;
    if (n_params > MAX_PARAMS || n_params < 0) {
        printf("[ERRO] Quantidade de parametros (%d) invalida ou excede o limite maximo (%d) na funcao '%s'.\n", n_params, MAX_PARAMS, name);
        return NULL;
    }
    if (symbol_table_func.n_funcs < MAX_FUNCS) {
        pos = symbol_table_func.n_funcs;
        strcpy(symbol_table_func.funcs[pos].name, name);
        sprintf(symbol_table_func.funcs[pos].label,"label_func_%s", name);
        symbol_table_func.funcs[pos].type = type;    
        for (i = 0; i < n_params; i++) {
            strcpy(symbol_table_func.funcs[pos].params[i].name, params[i].name);
            symbol_table_func.funcs[pos].params[i].type = params[i].type;            
        }
        symbol_table_func.funcs[pos].n_params = n_params;
        symbol_table_func.funcs[pos].implemented = 0;
        symbol_table_func.funcs[pos].called = 0;
        symbol_table_func.n_funcs++;
        return &symbol_table_func.funcs[pos];
    } else {
        printf("[ERRO] Limite de declaracao de funções atingido.\n");
    }
    return NULL;
}

/**
 * @brief Inicializa tabela de simbolos de variaveis globais
 * 
 * @param stv 
 */
void initSymbolTableVariables(type_symbol_table_variables *stv) {
    int i;
    stv->n_variables = 0;
    for (i = 0; i < MAX_SYMBOLS; i++) {
        strcpy(stv->variable[i].name, "");
        stv->variable[i].type = -1;
        stv->variable[i].addr = 0;
    }
    stv->next = NULL;
}

/**
 * @brief Inicializa tabela de simbolos de strings
 * 
 */
void initSymbolTableString() {
    int i;
    symbol_table_string.n_strings = 0;
    for (i = 0; i < MAX_SYMBOLS; i++) {
        strcpy(symbol_table_string.string[i].name, "");
        strcpy(symbol_table_string.string[i].value, "");
    }
}

/**
 * @brief Inicializa tabela de simbolos de funcoes
 * 
 */
void initSymbolTableFunc() {
    int i;
    symbol_table_func.n_funcs = 0;
    for (i=0; i<MAX_FUNCS; i++) {
        strcpy(symbol_table_func.funcs[i].name, "");
        symbol_table_func.funcs[i].type = 0;
        symbol_table_func.funcs[i].n_params = 0;
        symbol_table_func.funcs[i].implemented = 0;
        symbol_table_func.funcs[i].called = 0;
    }
}

///Para DEBUG
void printSTVariables(type_symbol_table_variables *stv) {
    int i, n_var;
    printf("TABELA DE SIMBOLOS DE VARIAVEIS:\n");
    n_var = stv->n_variables;
    printf("Numero de variaveis = %d\n", n_var);
    for (i = 0; i < n_var; i++) {
        printf("\tvariavel[%d].name:%s\n", i, stv->variable[i].name);
        if(stv->variable[i].type == INT){
            printf("\tvariavel[%d].type: INT | value: %d\n", i, *((int*)stv->variable[i].ptr_value));
        }
        else if(stv->variable[i].type == FLOAT){
            printf("\tvariavel[%d].type: FLOAT | value: %.2f\n", i, *((float*)stv->variable[i].ptr_value));  
        }
        else if(stv->variable[i].type == CHAR){
            printf("\tvariavel[%d].type: CHAR | value: '%c'\n", i, *((char*)stv->variable[i].ptr_value));
        }
        else if(stv->variable[i].type == STRING){
            printf("\tvariavel[%d].type: STRING | value: \"%s\"\n", i, ((char*)stv->variable[i].ptr_value));
        }
    }
}

void printSTString() {
    int i, n_str;
    printf("TABELA DE SIMBOLOS DE STRINGS:\n");
    n_str = symbol_table_string.n_strings;
    printf("Numero de strings = %d\n", n_str);
    for (i = 0; i < n_str; i++) {
        printf("\tstring[%d].name:%s\n", i, symbol_table_string.string[i].name);
        printf("\tstring[%d].value:%s\n", i, symbol_table_string.string[i].value);
    }
}

///Para DEBUG - Impressao da tabela de simbolos de funcoes
void printSTFunc() {
    int i, j, n_func;
    printf("TABELA DE SIMBOLOS DE FUNCOES:\n");
    n_func = symbol_table_func.n_funcs;
    printf("Numero de funcoes cadastradas = %d\n", n_func);
    for (i = 0; i < n_func; i++) {
        printf("\tFuncao[%d].name: %s\n", i, symbol_table_func.funcs[i].name);
        printf("\tFuncao[%d].tipo: %d\n", i, symbol_table_func.funcs[i].type);
        printf("\tFuncao[%d].n_param: %d\n", i, symbol_table_func.funcs[i].n_params);
        printf("\tFuncao[%d].implemented: %d\n", i, symbol_table_func.funcs[i].implemented);
        printf("\tFuncao[%d].called: %d\n", i, symbol_table_func.funcs[i].called);
        printf("\tFuncao[%d].parametros:\n", i);
        for (j=0; j < symbol_table_func.funcs[i].n_params; j++) {
            printf("\t\ttipo: %d\n", symbol_table_func.funcs[i].params[j].type);
            printf("\t\tnome: %s\n", symbol_table_func.funcs[i].params[j].name);
        }
    }
}