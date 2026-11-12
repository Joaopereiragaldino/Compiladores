/**
 * @file synt.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo do analisador sintatico
 * @version 0.4
 * 
 */

#include "synt.h"

// Variaveis globais
type_token *lookahead;
int erro_sintatico = false; // vira true em qualquer erro, para main nao imprimir 'sucesso'
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
extern type_symbol_table_func symbol_table_func;
extern char output_file_name[MAX_CHAR];
extern FILE *output_file;

/**
 * @brief Verifica se o proximo caracter na cadeia eh o esperado
 */
int match(int token_tag) {
    if ( lookahead->tag == token_tag ) {
        lookahead = getToken();
        return true;
    }
    printf("[ERRO] Entrada inesperada: '%s' (Tag recebida: %d | Tag esperada: %d)\n", lookahead->lexema, lookahead->tag, token_tag);
    erro_sintatico = true;
    return false;
}

/**
 * @brief Regra de derivacao inicial que coordena a geracao do arquivo .asm
 */
int program (void) {
    int ok1, ok2;
    gen_preambule();
    ok1 = declarations();
    gen_data_section();
    gen_preambule_code();

    // Delimita a funcao principal com begin e end
    if (lookahead->tag == BEGIN) {
        match(BEGIN);
    } else {
        printf("[ERRO] Esperado 'begin' para iniciar o bloco principal.\n");
        erro_sintatico = true;
    }

    ok2 = statements();

    if (lookahead->tag == END) {
        match(END);
    } else {
        printf("[ERRO] Esperado 'end' para encerrar o bloco principal.\n");
        erro_sintatico = true;
    }

    gen_epilog_code(); // Encerra a execucao do programa principal antes das funcoes
    func_codes();      // Compila o codigo das funcoes prototipadas

    return (ok1 == true) && (ok2 == true) && !erro_sintatico;
}

/**
 * @brief Regra de derivacao que implementa as funcoes prototipadas (Func_code)
 */
void func_codes(void) {
    while (lookahead->tag == INT || lookahead->tag == FLOAT || lookahead->tag == CHAR || lookahead->tag == STRING) {
        int var_type = lookahead->tag;
        if (!match(lookahead->tag)) return; // Consome o tipo
        
        char f_name[MAX_CHAR];
        strcpy(f_name, lookahead->lexema);
        if (!match(ID)) return; // Consome o nome da funcao

        // Verifica se a funcao foi prototipada
        type_symbol_table_func_entry *func = sym_func_find(f_name);
        if (func == NULL) {
            printf("[ERRO SEMANTICO] A funcao '%s' esta sendo implementada, mas nao foi declarada/prototipada.\n", f_name);
            erro_sintatico = true;
            return;
        }
        if (func->implemented) {
            printf("[ERRO SEMANTICO] A funcao '%s' ja foi implementada anteriormente.\n", f_name);
            erro_sintatico = true;
            return;
        }
        if (func->type != var_type) {
            printf("[ERRO] O tipo da funcao '%s' implementada (%d) difere do tipo do prototipo (%d).\n", f_name, var_type, func->type);
            erro_sintatico = true;
            return;
        }

        if (!match(OPEN_PAR)) return;

        // Compara os parametros da implementacao com o prototipo
        int cont_params = 0;
        while (lookahead->tag != CLOSE_PAR && lookahead->tag != ENDTOKEN) {
            int p_type = lookahead->tag;
            if (p_type == INT || p_type == FLOAT || p_type == CHAR || p_type == STRING) {
                if (!match(p_type)) return;

                char p_name[MAX_CHAR];
                strcpy(p_name, lookahead->lexema);
                if (!match(ID)) return;

                if (cont_params >= func->n_params) {
                    printf("[ERRO SEMANTICO] A implementacao da funcao '%s' possui mais parametros do que o prototipo (esperado: %d).\n",
                           f_name, func->n_params);
                    erro_sintatico = true;
                    return;
                }

                if (func->params[cont_params].type != p_type) {
                    printf("[ERRO SEMANTICO] Tipo do parametro %d ('%s') incompativel com o prototipo na funcao '%s'.\n",
                           cont_params + 1, p_name, f_name);
                    erro_sintatico = true;
                    return;
                }

                if (strcmp(func->params[cont_params].name, p_name) != 0) {
                    printf("[ERRO SEMANTICO] Nome do parametro %d ('%s') difere do nome no prototipo ('%s') na funcao '%s'.\n",
                           cont_params + 1, p_name, func->params[cont_params].name, f_name);
                    erro_sintatico = true;
                    return;
                }

                cont_params++;

                if (lookahead->tag == COMMA) {
                    if (!match(COMMA)) return;
                    if (lookahead->tag != INT && lookahead->tag != FLOAT && lookahead->tag != CHAR && lookahead->tag != STRING) {
                        printf("[ERRO] Erro nos parametros da funcao '%s'. Obrigatorio um tipo de dado apos a virgula.\n", f_name);
                        erro_sintatico = true;
                        return;
                    }
                } else {
                    break;
                }
            } else {
                printf("[ERRO] Parametro invalido na implementacao da funcao '%s'.\n", f_name);
                erro_sintatico = true;
                return;
            }
        }

        if (!match(CLOSE_PAR)) return;

        if (cont_params != func->n_params) {
            printf("[ERRO SEMANTICO] Quantidade de parametros na implementacao de '%s' (%d) difere do prototipo (%d).\n",
                   f_name, cont_params, func->n_params);
            erro_sintatico = true;
            return;
        }

        gen_label(func->label); // Grava o label da funcao no codigo Assembly

        if (lookahead->tag == BEGIN) {
            if (!match(BEGIN)) return;
        } else if (lookahead->tag == OPEN_BRACE) {
            if (!match(OPEN_BRACE)) return;
        } else {
            printf("[ERRO] Esperado 'begin' ou '{' para iniciar o corpo da funcao '%s'.\n", f_name);
            erro_sintatico = true;
            return;
        }

        statements(); // Analisa e compila os comandos internos da funcao

        if (lookahead->tag == END) {
            if (!match(END)) return;
        } else if (lookahead->tag == CLOSE_BRACE) {
            if (!match(CLOSE_BRACE)) return;
        } else {
            printf("[ERRO] Esperado 'end' ou '}' para finalizar o corpo da funcao '%s'.\n", f_name);
            erro_sintatico = true;
            return;
        }

        gen_jr(); // Gera instrucao de retorno (ret)
        func->implemented = 1;
    }

    // Verifica se alguma funcao chamada nao foi implementada
    int i;
    for (i = 0; i < symbol_table_func.n_funcs; i++) {
        if (symbol_table_func.funcs[i].called && !symbol_table_func.funcs[i].implemented) {
            printf("[ERRO SEMANTICO] Funcao '%s' foi chamada, mas nao foi implementada.\n", symbol_table_func.funcs[i].name);
            erro_sintatico = true;
        }
    }
}
/**
 * @brief Processa o bloco continuo de declaracoes de variaveis
 */
int declarations(void) {
    int flag;
    //Laco para processamento continuo das declaracoes
    do {
        flag = declaration();
    } while ( flag == true );
    if (flag == false)
        return true;
    else
        return flag; 
}

/**
 * @brief Processa a declaracao de uma variavel individual (int, float, char, string)
 */
int declaration (void) {
    type_symbol_table_entry *search_symbol;
    type_symbol_table_func_entry *search_func_symbol;
    int ok1, ok2;
    char var_name[MAX_CHAR];
    int var_type;

    // Verifica o tipo da variavel
    var_type = lookahead->tag;
    if ( var_type == INT || var_type == FLOAT || var_type == CHAR || var_type == STRING ) { 
        match(var_type);
        
        if (lookahead->tag == ID) {
            strcpy(var_name, lookahead->lexema);
            search_symbol = sym_find( var_name, &global_symbol_table_variables );

            if( search_symbol != NULL) {
                printf ("[ERRO] Variavel '%s' ja declarada.\n", var_name); 
                erro_sintatico = true;
                return false;
            } 
            ok1 = match(ID);

            if(lookahead->tag == SEMICOLON) {
                void *value_initial = NULL;
                
                // Alocacao de valor inicial baseado no tipo genérico
                if (var_type == INT) {
                    value_initial = malloc(sizeof(int));
                    *((int*)value_initial) = 0;
                } else if (var_type == FLOAT) {
                    value_initial = malloc(sizeof(float));
                    *((float*)value_initial) = 0.0f;
                } else if (var_type == CHAR) {
                    value_initial = malloc(sizeof(char));
                    *((char*)value_initial) = '\0';
                } else if (var_type == STRING) {
                    value_initial = malloc(MAX_CHAR * sizeof(char));
                    strcpy((char*)value_initial, "");
                }

                sym_declare( var_name, var_type, 0, value_initial, &global_symbol_table_variables);
                
                ok2 = match(SEMICOLON);
                return ok1 && ok2;
            } // Declaracao de FUNCAO
            else if(lookahead->tag == OPEN_PAR){ // Verifica se eh declaracao de funcao
            search_func_symbol = sym_func_find(var_name);
            if(search_func_symbol != NULL){
                printf("[ERRO] Funcao '%s' ja declarada.\n", var_name);
                erro_sintatico = true;
                return false;
            } 
            else {
                if (!match(OPEN_PAR)) return false;
                //Constroi a lista de parametros
                type_symbol_table_entry parameters[MAX_PARAMS];
                int n_params;
                n_params = 0;
                char p_name[MAX_CHAR];
                int p_type;

                while(lookahead->tag != CLOSE_PAR && lookahead->tag != ENDTOKEN){
                    p_type = lookahead->tag;
                    if(p_type == INT || p_type == FLOAT || p_type == CHAR || p_type == STRING){
                        if (n_params >= MAX_PARAMS) {
                            printf("[ERRO] Quantidade de parametros excede o limite maximo (%d) na declaracao da funcao '%s'.\n", MAX_PARAMS, var_name);
                            erro_sintatico = true;
                            return false;
                        }
                        if (!match(p_type)) return false;
                        strcpy(p_name, lookahead->lexema);
                        if (!match(ID)) return false;

                        if (sym_find(p_name, &global_symbol_table_variables) != NULL) {
                            printf("[ERRO] Parametro '%s' da funcao '%s' ja existe como variavel/parametro global (parametros sao globais).\n", p_name, var_name);
                            erro_sintatico = true;
                            return false;
                        }

                        parameters[n_params].type = p_type;
                        strcpy(parameters[n_params].name, p_name);
                        n_params++;

                        void *val_init = NULL;
                        if (p_type == INT)        { val_init = malloc(sizeof(int));   *((int*)val_init) = 0; }
                        else if (p_type == FLOAT) { val_init = malloc(sizeof(float)); *((float*)val_init) = 0.0f; }
                        else if (p_type == CHAR)  { val_init = malloc(sizeof(char));  *((char*)val_init) = '\0'; }
                        else                      { val_init = malloc(MAX_CHAR);      strcpy((char*)val_init, ""); }
                        sym_declare(p_name, p_type, 0, val_init, &global_symbol_table_variables);

                        if(lookahead->tag == COMMA){
                            if (!match(COMMA)) return false;
                            if(lookahead->tag != INT && lookahead->tag != FLOAT && lookahead->tag != CHAR && lookahead->tag != STRING){
                                printf("[ERRO] Erro na declaracao de parametro da funcao. Obrigatorio um tipo de dado apos a virgula.\n");
                                erro_sintatico = true;
                                return false;
                            }
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                if (!match(CLOSE_PAR)) return false;
                if (!match(SEMICOLON)) return false;
                // Cadastra efetivamente a funcao na TSF com seus parametros
                if (sym_func_declare(var_name, var_type, parameters, n_params) == NULL) {
                    erro_sintatico = true;
                    return false;
                }
                return true;
            }
            } 
            else {
                printf("[ERRO] Era esperado um ';' ou '(' para reconhecer declaracao de variavel, ou de funcao.\n");
                erro_sintatico = true;
                return false;
            }
        } 
        else {
            printf("[ERRO] Identificador de variavel esperado.\n");
            erro_sintatico = true;
            return false;
        }
    } 
    else if(var_type == IF || 
            var_type == WHILE || 
            var_type == READ || 
            var_type == WRITE || 
            var_type == ID || 
            var_type == ENDTOKEN || 
            var_type == END || 
            var_type == CLOSE_BRACE ||
            var_type == BEGIN) {
        return false;         
    } else {
        printf ("[ERRO] Tipo desconhecido na declaracao: %d %s.\n", lookahead->tag, lookahead->lexema);
        erro_sintatico = true;
        return false; 
    }
}

/**
 * @brief Processa o bloco continuo de comandos
 */
int statements (void) {
    int flag;
    //processa enquanto houver comandos
    do {
        flag = statement();
    } while ( flag == true );
    return lookahead->tag == ENDTOKEN ||
           lookahead->tag == END ||
           lookahead->tag == CLOSE_BRACE;
}
/**
 * @brief Processa os comandos de Atribuicao (ID = CONSTANTE ;)
 */
int statement (void) {
    char lexeme_of_id[MAX_CHAR];
    type_symbol_table_entry *search_symbol;
    type_symbol_table_func_entry *search_symbol_func;
    int ok1, ok2, ok3, ok4;
    char val_lexema[MAX_CHAR];

    if (lookahead->tag == ENDTOKEN || lookahead->tag == END || lookahead->tag == CLOSE_BRACE) {
        return false;
    }
    else if (lookahead->tag == READ){
        match(READ);
        strcpy(lexeme_of_id, lookahead->lexema);
        ok1 = match(ID);
        ok2 = match(SEMICOLON);

        if(ok1 && ok2){
            search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
            if(search_symbol != NULL){
                gen_read(lexeme_of_id);
                return true;
            }
            else {
                printf("[ERRO] Leitura de variável não declarada: '%s'\n", lexeme_of_id); erro_sintatico = true;
                return false;
            }
        }
        else {
            printf("[ERRO] Falha no reconhecimento da estrutura do READ, era esperado ID e ';'\n"); erro_sintatico = true;
            return false;
        }
    }
    else if(lookahead->tag == WRITE){
        match(WRITE);
        strcpy(lexeme_of_id, lookahead->lexema);
        ok1 = match(ID);
        ok2 = match(SEMICOLON);

        if(ok1 && ok2){
            search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
            if(search_symbol != NULL){
                gen_write(lexeme_of_id);
                return true;
            } else {
                printf("[ERRO] Escrita de variável não declarada: '%s'\n", lexeme_of_id); erro_sintatico = true;
                return false;
            }
        }
        else {
            printf("[ERRO] Falha no reconhecimento da estrutura do WRITE, era esperado ID e ';'\n"); erro_sintatico = true;
            return false;
        }
    }
    else if (lookahead->tag == ID) { 
        char id_name[MAX_CHAR];
        strcpy(id_name,lookahead->lexema);
        match(ID);

        if(lookahead->tag == ASSIGN){
            match(ASSIGN);
            search_symbol = sym_find(id_name, &global_symbol_table_variables);

            if (search_symbol == NULL) {
                printf("[ERRO] Atribuicao invalida: Variavel '%s' nao declarada.\n", lexeme_of_id); 
                erro_sintatico = true;
                return false;
            }

            // O valor atribuido depende do tipo da variavel
            int tipo = search_symbol->type;
            int tag  = lookahead->tag;

            if (tipo == STRING) {
                if (tag != STRING_VAL) {
                    printf("[ERRO] Valor incompativel com a variavel string '%s'.\n", lexeme_of_id);
                    erro_sintatico = true;
                    return false;
                }
                strcpy((char*)search_symbol->ptr_value, lookahead->lexema);
                genAssignString(id_name, lookahead->lexema);
                if (!match(STRING_VAL)) return false;
            }
            else if (tipo == CHAR) {
                if (tag != CHAR_VAL) {
                    printf("[ERRO] Valor incompativel com a variavel char '%s' (esperado: 'x').\n", lexeme_of_id);
                    erro_sintatico = true;
                    return false;
                }
                *((char*)search_symbol->ptr_value) = lookahead->lexema[0];
                genAssignChar(id_name, lookahead->lexema);
                if (!match(CHAR_VAL)) return false;
            }
            else if (tipo == FLOAT) {
                char val_float[MAX_CHAR];
                if (tag != NUM_FLOAT && tag != NUM_INT) {
                    printf("[ERRO] Valor incompativel com a variavel float '%s' (esperado: numero).\n", lexeme_of_id);
                    erro_sintatico = true;
                    return false;
                }
                strcpy(val_float, lookahead->lexema);
                if (tag == NUM_INT) strcat(val_float, ".0"); 
                *((float*)search_symbol->ptr_value) = (float) atof(val_float);
                genAssignFloat(id_name, val_float);
                if (!match(tag)) return false;
            }
            else { // INT
                if (tag == STRING_VAL || tag == NUM_FLOAT) {
                    printf("[ERRO] Valor incompativel com a variavel int '%s'.\n", lexeme_of_id);
                    erro_sintatico = true;
                    return false;
                }
                if (tag == NUM_INT && peekChar() == SEMICOLON) {
                    // Atribuicao de constante simples: a = 5;
                    *((int*)search_symbol->ptr_value) = atoi(lookahead->lexema);
                    genAssignInt(id_name, lookahead->lexema);
                    if (!match(NUM_INT)) return false;
                } else if (E()) {
                    // Expressao aritmetica: o resultado esta no topo da pilha
                    gen_assign_var(id_name);
                } else {
                    printf("[ERRO] Expressao invalida para a variavel '%s'.\n", lexeme_of_id);
                    erro_sintatico = true;
                    return false;
                }
            }
            return match(SEMICOLON);
        }
        // Verifica se eh uma chamada de funcao
        else if(lookahead->tag == OPEN_PAR){
            match(OPEN_PAR);

            search_symbol_func = sym_func_find(id_name);
            if (search_symbol_func == NULL) {
                printf("[ERRO] Tentativa de chamada de funcao nao declarada: '%s'\n", id_name);
                erro_sintatico = true;
                return false;
            }
            
            int cont_params = 0;
            int erro_semantico = false;

            while(lookahead->tag != CLOSE_PAR && lookahead->tag != ENDTOKEN){
                char lexema_arg[MAX_CHAR];
                int arg_tag = lookahead->tag;
                char *param_name;
                int param_type;
                strcpy(lexema_arg, lookahead->lexema);

                if (cont_params >= search_symbol_func->n_params) {   // argumentos demais
                    printf("[ERRO SEMANTICO] Chamada de '%s' com argumentos demais (o prototipo declara %d).\n",
                           id_name, search_symbol_func->n_params);
                    erro_semantico = true;
                    while (lookahead->tag != CLOSE_PAR && lookahead->tag != ENDTOKEN && lookahead->tag != SEMICOLON) {
                        lookahead = getToken();
                    }
                    break;
                }
                param_name = search_symbol_func->params[cont_params].name;
                param_type = search_symbol_func->params[cont_params].type;

                if (arg_tag == ID && (peekChar() == ',' || peekChar() == ')')) {
                    // id puro: MESMO id e mesmo tipo do parametro. O valor ja esta na global: nao gera codigo.
                    type_symbol_table_entry *var = sym_find(lexema_arg, &global_symbol_table_variables);
                    if (var == NULL) {
                        printf("[ERRO SEMANTICO] Variavel '%s' nao declarada usada como parametro.\n", lexema_arg);
                        erro_semantico = true;
                    } else if (strcmp(lexema_arg, param_name) != 0) {
                        printf("[ERRO SEMANTICO] Chamada de '%s': o argumento %d deveria ter o id '%s' (do prototipo), mas foi '%s'.\n",
                               id_name, cont_params + 1, param_name, lexema_arg);
                        erro_semantico = true;
                    } else if (var->type != param_type) {
                        printf("[ERRO SEMANTICO] Parametro %d incompativel na chamada da funcao '%s'.\n", cont_params + 1, id_name);
                        erro_semantico = true;
                    }
                    match(ID);
                }
                else if (arg_tag == NUM_INT && (peekChar() == ',' || peekChar() == ')') && param_type == INT) {
                    genAssignInt(param_name, lexema_arg);
                    match(NUM_INT);
                }
                else if ((arg_tag == NUM_FLOAT || arg_tag == NUM_INT) && param_type == FLOAT) {
                    char val_float[MAX_CHAR + 4];
                    strcpy(val_float, lexema_arg);
                    if (arg_tag == NUM_INT) strcat(val_float, ".0");
                    genAssignFloat(param_name, val_float);
                    match(arg_tag);
                }
                else if (arg_tag == CHAR_VAL && param_type == CHAR) {
                    genAssignChar(param_name, lexema_arg);
                    match(CHAR_VAL);
                }
                else if (arg_tag == STRING_VAL && param_type == STRING) {
                    genAssignString(param_name, lexema_arg);
                    match(STRING_VAL);
                }
                else if (param_type == INT && arg_tag != STRING_VAL && arg_tag != NUM_FLOAT && arg_tag != CHAR_VAL) {
                    if (E()) {                      // expressao: resultado no topo da pilha
                        gen_assign_var(param_name);
                    } else {
                        printf("[ERRO] Expressao invalida no argumento %d da chamada de '%s'.\n", cont_params + 1, id_name);
                        erro_semantico = true;
                        while (lookahead->tag != CLOSE_PAR && lookahead->tag != ENDTOKEN && lookahead->tag != SEMICOLON) {
                            lookahead = getToken();
                        }
                        break;
                    }
                }
                else {
                    printf("[ERRO SEMANTICO] Parametro %d incompativel na chamada da funcao '%s'.\n", cont_params + 1, id_name);
                    erro_semantico = true;
                    while (lookahead->tag != CLOSE_PAR && lookahead->tag != ENDTOKEN && lookahead->tag != SEMICOLON) {
                        lookahead = getToken();
                    }
                    break;
                }
                cont_params++;

                if (lookahead->tag == COMMA) {
                    match(COMMA);
                } else {
                    break;
                }
            }

            // Verifica a quantidade total de parametros comparando com o prototipo (TSF)
            if (!erro_semantico && cont_params != search_symbol_func->n_params) {
                printf("[ERRO SEMANTICO] Quantidade de parametros incorreta na chamada de '%s'. Esperado: %d, Informado: %d.\n", 
                       id_name, search_symbol_func->n_params, cont_params);
                erro_semantico = true;
            }

            if (!match(CLOSE_PAR)){
                printf("[ERRO] Era esperado ')' no fechamento da funcao: '%s' \n", id_name);
                erro_sintatico = true;
                return false;
            }
            
            int ok_semi = match(SEMICOLON);

            if (!erro_semantico && ok_semi) {
                search_symbol_func->called = 1;
                gen_jal(search_symbol_func->label); // Gera a instrucao de salto (jal / call)
                return true;
            } else {
                erro_sintatico = true;
                return false;
            }
        }
        else {
            printf("[ERRO] Esperado '=' ou '(' apos o identificador '%s'.\n", id_name);
            erro_sintatico = true;
            return false;
        }
    }
    else if (lookahead->tag == IF) {
        char label_else[MAX_CHAR], label_end[MAX_CHAR];
        gen_label_name(label_else);
        gen_label_name(label_end);

        if(!match(IF)){
            return false;
        } 
        if(!match(OPEN_PAR)){
            return false;
        }
        
        // Se a expressao booleana estiver mal formada, emite erro
        if (!B()) {
            printf("[ERRO] Expressao mal formada no laco IF.\n"); erro_sintatico = true;
            return false;
        }
        
        if(!match(CLOSE_PAR)){
            return false;
        }
        gen_cond_jump(label_else);
        
        // Aceita tanto BEGIN/END quanto chaves { }
        if (lookahead->tag == BEGIN) {
            if (!match(BEGIN)) return false;
        } 
        else {
            if (!match(OPEN_BRACE)) return false;
        }

        // comandos de dentro
        if (!statements()) return false;

        // fechamento do bloco do if
        if (lookahead->tag == END) {
            if (!match(END)) return false;
        } 
        else {
            if (!match(CLOSE_BRACE)) return false;
        }

        gen_incond_jump(label_end);
        gen_label(label_else);

        if (lookahead->tag == ELSE) {
            if (!match(ELSE)) {
            return false;
            }
            // abertura do bloco do else
            if (lookahead->tag == BEGIN) {
                if (!match(BEGIN)) return false;
            } 
            else {
                if (!match(OPEN_BRACE)) return false;
            }

        if (!statements()){
            return false;
        }

        // fechamento do bloco do else
        if (lookahead->tag == END) {
            if (!match(END)){
                return false;
            }
        } 
        else {
            if (!match(CLOSE_BRACE)){
                return false;
            }
        }
    }
        gen_label(label_end);
        return true; // Retorna true para continuar processando proximos statements
    }
    else if (lookahead->tag == WHILE) {
    char label_begin[MAX_CHAR], label_end[MAX_CHAR];

    gen_label_name(label_begin);
    gen_label_name(label_end);

    if (!match(WHILE)){   
        return false;
    }
    if (!match(OPEN_PAR)){ 
        return false;
    }

    gen_label(label_begin);

    if (!B()) {
        printf("[ERRO] Expressao mal formada no WHILE.\n"); erro_sintatico = true;
        return false;
    }

    if (!match(CLOSE_PAR)){ 
        return false;
    }
    gen_cond_jump(label_end);

    // abertura do bloco
    if (lookahead->tag == BEGIN) {
        if (!match(BEGIN)){
            return false;
        }
    } 
    else {
        if (!match(OPEN_BRACE)){
            return false;
        }
    }

    if (!statements()){ 
        return false;
    }

    // fechamento do bloco
    if (lookahead->tag == END) {
        if (!match(END)){
            return false;
        }
    } 
    else {
        if (!match(CLOSE_BRACE)){
            return false;
        }
    }

    gen_incond_jump(label_begin);
    gen_label(label_end);
    return true;
    }
    
    printf("[ERRO] Comando desconhecido: '%s'\n", lookahead->lexema); erro_sintatico = true;
    return false;
}



/**
 * @brief Regra de derivação que analiza expressoes booleanas
 *        no padrao 'id op_rel expr'
 * 
 */
////// TODO
int B() {
    int b1 = B_term(); // Avalia lado esquerdo
    while (lookahead->tag == LG_OR) { // && e || tem prioridades diferentes
        match(LG_OR);
        int b2 = B_term(); // Avalia lado direito
        gen_or();
        b1 = b1 && b2;
    }
    return b1;
}

int B_term() {
    int b1 = B_fact();
    while (lookahead->tag == LG_AND) {
        match(LG_AND);
        int b2 = B_fact();
        gen_and();
        b1 = b1 && b2;
    }
    return b1;
}

int B_fact() {
    int operator;
    
    // Processa Expressoes Aritmeticas/Variaveis do Lado Esquerdo da comparacao
    int ok1 = E(); 
    
    if (boolOperator(&operator)) {
        // Processa Expressoes Aritmeticas/Variaveis do Lado Direito da comparacao
        int ok2 = E(); 
        
        if (ok1 && ok2) {
            gen_bool(operator);
            return true;
        }
    }
    return false;
}

int boolOperator(int *operator) {
    int lookahead_tag;
    lookahead_tag = lookahead->tag;
    *operator = -1;
    
    if (lookahead_tag == EQUAL ||
        lookahead_tag == NE ||
        lookahead_tag == LT ||
        lookahead_tag == GT ||
        lookahead_tag == LE ||
        lookahead_tag == GE) {
            *operator = lookahead_tag;
            match(lookahead_tag);
            return true;
    } else {
        printf("[ERRO] Operador relacional esperado.\n");
        erro_sintatico = true;
        return false;
    }
}

//------
/* Funções que representam a gramatica que reconhece expressoes aritmeticas */
/* Elaborada nas primeiras aulas */
int E() {
    int b1, b2 = false;
    b1 = T();
    if (b1) 
        b2 = ER();
    return b1 && b2;
}

int ER() {
    if (lookahead->tag == PLUS)  { 
        int b1, b2 = false;
        match(PLUS);
        b1 = T();
        genAdd();
        if (b1) 
            b2 = ER();
        return b1 && b2;
    } else if (lookahead->tag == MINUS) {
        int b1, b2 = false;
        match(MINUS);
        b1 = T();
        genSub();
        if (b1)
            b2 = ER();
        return b1 && b2;      
    } else if (lookahead->tag == CLOSE_PAR   ||
               lookahead->tag == ENDTOKEN    ||
               lookahead->tag == SEMICOLON   ||
               lookahead->tag == EQUAL       ||
               lookahead->tag == NE          ||
               lookahead->tag == LT          ||
               lookahead->tag == GT          ||
               lookahead->tag == LE          ||
               lookahead->tag == GE          ||
               lookahead->tag == LG_AND      ||
               lookahead->tag == LG_OR       ||
               lookahead->tag == COMMA) {
        return true;
    } else {
        return false;
    }
}

int T() {
    int b1, b2 = false;
    b1 = F();
    if (b1)
        b2 = TR();
    return b1 && b2;
}

int TR() {
    if (lookahead->tag == MULT) { 
        int b1, b2 = false;
        match(MULT);
        b1 = F();
        genMult();
        if (b1)
            b2 = TR();
        return b1 && b2;
    } else if (lookahead -> tag == DIV) {
        int b1, b2 = false;
        match(DIV);
        b1 = F();
        genDiv();
        if (b1)
            b2 = TR();
        return b1 && b2;
    } else if (lookahead->tag == PLUS        ||
               lookahead->tag == MINUS       ||
               lookahead->tag == CLOSE_PAR   ||
               lookahead->tag == ENDTOKEN    ||
               lookahead->tag == SEMICOLON   ||
               lookahead->tag == EQUAL       ||
               lookahead->tag == NE          ||
               lookahead->tag == LT          ||
               lookahead->tag == GT          ||
               lookahead->tag == LE          ||
               lookahead->tag == GE          ||
               lookahead->tag == LG_AND      ||
               lookahead->tag == LG_OR       ||
               lookahead->tag == COMMA) 
    {
        return true;
    } 
    else {
        return false;
    }
}

int F() {
    if (lookahead->tag == OPEN_PAR) {
        int b1, b2 = false;
        match(OPEN_PAR);
        b1 = E();
        if (b1)
            b2 = match(CLOSE_PAR);
        return b1 && b2;
    } else if (lookahead->tag == NUM_INT) {
        int b1;
        char lexema[MAX_TOKEN];
        strcpy(lexema, lookahead->lexema);
        b1 = match(NUM_INT);
        genNum(lexema);
        return b1;
    } else if (lookahead->tag == NUM_FLOAT) {
        // As contas sao feitas com inteiros (registradores de 64 bits)
        printf("[ERRO] Numero float '%s' nao e suportado em expressoes.\n", lookahead->lexema);
        erro_sintatico = true;
        return false;
    } else if (lookahead->tag == CHAR_VAL) {
        // 'a' vira o codigo ASCII (97), permitindo c == 'a'
        int b1;
        char lexema[MAX_TOKEN];
        sprintf(lexema, "%d", (unsigned char) lookahead->lexema[0]);
        b1 = match(CHAR_VAL);
        genNum(lexema);
        return b1;
    } else if (lookahead->tag == ID) {
        char lexema[MAX_TOKEN];
        type_symbol_table_entry *sym;
        int b1;
        strcpy(lexema, lookahead->lexema);
        sym = sym_find(lexema, &global_symbol_table_variables);
        if (sym == NULL) {
            printf("[ERRO] Variavel '%s' nao declarada.\n", lexema);
            erro_sintatico = true;
            return false;
        }
        if (sym->type == FLOAT || sym->type == STRING) {
            printf("[ERRO] Variavel '%s' (float/string) nao pode ser usada em expressoes.\n", lexema);
            erro_sintatico = true;
            return false;
        }
        b1 = match(ID);
        if (sym->type == CHAR)
            gen_push_char(lexema);  // le so 1 byte
        else
            gen_push_var(lexema);   // le 4 bytes (int)
        return b1;
    } else {
        return false;
    }
}
/*fim da importação */
//------

//--------------------- MAIN -----------------------
/**
 * @brief Funcao principal (main) do compilador
 * 
 * @return int 
 */
int main(int argc, char *argv[]) {

    // Inicializa a tabela de simbolo global
    initSymbolTableVariables(&global_symbol_table_variables);
    initSymbolTableString();
    initSymbolTableFunc();

    int sucess;

    // Verifica a passagem de parametro
    if (argc != 2) {
        printf("[ERRO]\n\tÉ necessário informar um arquivo de entrada (código) como parâmetro.\n\n");
        exit(EXIT_FAILURE);
    }
    
    initLex(argv[1]); // Carrega o codigo
    lookahead = getToken(); // Inicializacao do lookhead

    // Abre o arquivo de saida 
    strcpy(output_file_name, argv[1]);
    strcat(output_file_name, ".asm");
    output_file = fopen(output_file_name, "w+");

    sucess = program(); // Chamada da derivacao/funcao inicial da gramatica

    if (sucess == true && !erro_sintatico)
        printf("Compilacao realizada com sucesso!\n");
    else
        printf("[ERRO] Falha na compilacao!\n");
    
    printf("\n\n***Impressao da tabela de simbolos***\n");
    printSTVariables(&global_symbol_table_variables);
    printSTFunc();
    fclose(output_file);
    if (sucess == true && !erro_sintatico){
        return 0;
    } else {
        return 1;
    }
}