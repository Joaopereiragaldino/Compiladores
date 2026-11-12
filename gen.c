/**
 * @file gen.c
 * @author Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo gerador de codigo
 * @version 0.2
 * @date 2022-02-04
 * 
 */

// Inclusao do cabecalho
#include "gen.h"

// Inclusao explicita de variaveis globais de outro contexto (symbols.h)
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
char output_file_name[MAX_CHAR];
FILE *output_file;

/**
 * @brief Funcao que gera codigo de montagem para SOMA
 * 
 */
void genAdd() {
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tpop rbx\n");
    fprintf(output_file, "\tadd rax,rbx\n");
    fprintf(output_file, "\tpush rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para SUBTRACAO
 * 
 */
void genSub() {
    fprintf(output_file, "\tpop rbx\n");
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tsub rax,rbx\n");
    fprintf(output_file, "\tpush rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para MULTIPLICACAO
 * 
 */
void genMult() {
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tpop rbx\n");
    fprintf(output_file, "\timul rax,rbx\n");
    fprintf(output_file, "\tpush rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para DIVISAO
 * 
 */
void genDiv() {
    fprintf(output_file, "\tpop rbx\n");
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tcqo\n");
    fprintf(output_file, "\tidiv rbx\n");
    fprintf(output_file, "\tpush rax\n");
}

void gen_and(void) {
    fprintf(output_file, "\t; AND Logico (&&)\n");
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tpop rbx\n");
    fprintf(output_file, "\tand rax, rbx\n");
    fprintf(output_file, "\tpush rax\n");
}

void gen_or(void) {
    fprintf(output_file, "\t; OR Logico (||)\n");
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tpop rbx\n");
    fprintf(output_file, "\tor rax, rbx\n");
    fprintf(output_file, "\tpush rax\n");
}

/**
 * @brief Gera o salto para chamada de funcao, salvando retorno
 */
void gen_jal(char *label) {
    fprintf(output_file, "\t;chamada de funcao\n");
    fprintf(output_file, "\tcall %s\n", label);
}

/**
 * @brief Gera o retorno da funcao para o chamador
 */
void gen_jr(void) {
    fprintf(output_file, "\t;retorno da funcao\n");
    fprintf(output_file, "\tret\n");
}

/**
 * @brief Funcao que gera codigo de montagem para armazenamento de NUMERAL
 * 
 * @param num_string 
 */
void genNum(char num_string[MAX_TOKEN]) {
    fprintf(output_file, "\t; Armazenamento de numero\n");
    fprintf(output_file, "\tmov rax,%s\n", num_string);
    fprintf(output_file, "\tpush rax\n");
}

/**
 * @brief Funcao que gera um preambulo que permite o uso das funcoes do C (scanf e printf)
 * 
 */
void gen_preambule(void) {
    fprintf(output_file, ";UFMT-Compiladores\n");
    fprintf(output_file, ";Prof. Ivairton\n");
    fprintf(output_file, ";Procedimento para geracao do executavel apos compilacao (em Linux):\n");
    fprintf(output_file, ";(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>\n");
    fprintf(output_file, ";(2) likedicao: $ ld -m elf_x86_64 <nome_arquivo_objeto>\n\n");
    fprintf(output_file, "extern printf\n");
    fprintf(output_file, "extern scanf\n");
}

/**
 * @brief Funcao que gera codigo da secao de dados (declaracao de variaveis).
 */
void gen_data_section(void) {
    int i, n;
    
    fprintf(output_file, "section .data\n");

    // emite strings de formato fixo
    fprintf(output_file, "fmtstr0  db  \"%%d\",0\n");
    fprintf(output_file, "fmtstr1  db  \"%%s\",0\n");
    
    // processa cada simbolo da tabela e gera um ponteiro para cada variavel na memoria
    n = global_symbol_table_variables.n_variables;
    for (i = 0; i < n; i++) {
       fprintf(output_file, "%s  ", global_symbol_table_variables.variable[i].name); 
       
       switch(global_symbol_table_variables.variable[i].type) { //Por enquanto gera endereco zero
           case INT:
               fprintf(output_file, "dd  0\n");
               break;
            case FLOAT:
                fprintf(output_file, "dd 0.0\n");
                break;
            case CHAR:
                fprintf(output_file, "db  0\n");
               break;
           case STRING:
               fprintf(output_file, "db  \"                \" \n");
               break;
           default:
               fprintf(output_file, "[ERRO] Tipo desconhecido.\n");       
               break;           
       }
    }

    n = symbol_table_string.n_strings;
    for (i = 0; i < n; i++) {
        fprintf(output_file, "%s  db  %s\n", symbol_table_string.string[i].name, symbol_table_string.string[i].value);
    }
}

/**
 * @brief Funcao que gera a marcacao do inicio da secao de codigo
 * 
 */
void gen_preambule_code(void) {
    fprintf(output_file, "\n\tsection .text\n");
    fprintf(output_file, "\tglobal main,_start\n");
    fprintf(output_file, "main:\n");
    fprintf(output_file, "_start:\n");
}

/**
 * @brief Funcao que encerra o codigo inserindo comandos de fechamento
 * 
 */
void gen_epilog_code(void) {
    //fprintf(output_file, "\nret\n");
    fprintf(output_file, "\n\t;encerra programa\n");
    fprintf(output_file, "\tmov ebx,0\n");
    fprintf(output_file, "\tmov eax,1\n");
    fprintf(output_file, "\tint 0x80\n");
}


/**
 * @brief Funcao que gera automaticamente um novo nome para um label
 * @param string name
 */
void gen_label_name( char *name ) {
    char str_name[MAX_CHAR];
    char conv_value[16];
    static int nlabels=0;

    sprintf(conv_value, "%d", nlabels);
    strcpy(str_name, "label");
    strcat(str_name, conv_value);
    strcpy(name, str_name);
    nlabels++;
}


/**
 * @brief Funcao que registra no codigo um label
 * @param string label 
 */
void gen_label( char *label ) {
    fprintf(output_file, "%s:\n", label); 
}

/**
 * @brief Funcao que gera um jump condicional
 * @param string label
 */
void gen_cond_jump(char *label) {
    fprintf(output_file, "\t;jump condicional\n");
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tcmp rax, 0\n");
    fprintf(output_file, "\tjz %s\n", label);
}


/**
 * @brief Funcao que gera um jump incondicional
 * @param string label
 */
void gen_incond_jump(char *label) {
    fprintf(output_file, "\t;jump incondicional\n");
    fprintf(output_file, "\tjmp %s\n", label);
}


/**
 * @brief Funcao que gera codigo a partir do processamento da expressao logica
 * 
 * @param oper 
 */
void gen_bool(int oper) {
    char bool_label_name[MAX_CHAR];
    
    fprintf(output_file, "\t; Aplica operador booleano/exp.logica\n");
    fprintf(output_file, "\tpop rbx\n"); // Lado direito
    fprintf(output_file, "\tpop rax\n"); // Lado esquerdo 
    
    gen_bool_label_name(bool_label_name);
    
    fprintf(output_file, "\tmov rcx, 1\n");
    fprintf(output_file, "\tcmp rax, rbx\n"); 
    
    switch (oper) {
        case EQUAL:
            fprintf(output_file, "\tje %s\n", bool_label_name);
            break;
        case NE:
            fprintf(output_file, "\tjne %s\n", bool_label_name);
            break;
        case LT:
            fprintf(output_file, "\tjl %s\n", bool_label_name);
            break;
        case GT:
            fprintf(output_file, "\tjg %s\n", bool_label_name);
            break;
        case LE:
            fprintf(output_file, "\tjle %s\n", bool_label_name);
            break;
        case GE:
            fprintf(output_file, "\tjge %s\n", bool_label_name);
            break;
        default:
            printf("[ERRO] operador booleano nao suportado.\n");
            break;
    }
    
    fprintf(output_file, "\tmov rcx, 0\n");
    gen_label(bool_label_name); // Imprime o label do salto 
    fprintf(output_file, "\tmov rax, rcx\n");
    fprintf(output_file, "\tpush rax\n");
}

void gen_bool_label_name(char *name) {
    char str_name[MAX_CHAR];
    char conv_value[16];
    static int nbool_labels = 0;
    sprintf(conv_value, "%d", nbool_labels);
    
    strcpy(str_name, "label_bool"); 
    strcat(str_name, conv_value);
    strcpy(name, str_name);
    nbool_labels++;
}


/**
 * @brief Gera codigo para atribuir um valor calculado na expressao (no topo da pilha) a variavel
 */
void gen_assign_var(char *id) {
    fprintf(output_file, "\t; Atribuicao de valor em variavel\n");
    fprintf(output_file, "\tpop rax\n");
    fprintf(output_file, "\tmov dword [%s], eax\n", id);
}

/**
 * @brief Empilha o valor da variavel para contas e comparacoes
 */
void gen_push_var(char *id) {
    fprintf(output_file, "\t; Empilha variavel %s\n", id);
    fprintf(output_file, "\tmov eax, dword [%s]\n", id);
    fprintf(output_file, "\tpush rax\n");
}



/**
 * @brief Empilha o valor de uma variavel CHAR (1 byte) estendido com zeros
 */
void gen_push_char(char *id) {
    fprintf(output_file, "\t; Empilha variavel char %s\n", id);
    fprintf(output_file, "\tmovzx eax, byte [%s]\n", id);
    fprintf(output_file, "\tpush rax\n");
}

// Emissao de instrucoes de Atribuicao no arquivo NASM (.asm)
void genAssignInt(char *id, char *value) {
    fprintf(output_file, "\tmov dword [%s], %s\n", id, value);
}

void genAssignFloat(char *id, char *value) {
    fprintf(output_file, "\tmov dword [%s], __float32__(%s)\n", id, value);
}

void genAssignChar(char *id, char *value) {
    fprintf(output_file, "\tmov byte [%s], '%c'\n", id, value[0]);
}

void genAssignString(char *id, char *value) {
    fprintf(output_file, "\t; Atribuicao de string para %s: %s\n", id, value);
}

/**
 * @brief Gera codigo para o comando READ
 * 
 * @param lexeme_of_id nome do identificador
 */
void gen_read(char *lexeme_of_id) {
    /* Para o contexto de uso do comando SCANF:
    fprintf(output_file, "mov rdi, fmtstr0\n");
    fprintf(output_file, "mov rsi, %s\n", lexeme_of_id);
    fprintf(output_file, "mov rax, 0\n");
    fprintf(output_file, "call scanf\n");*/
    
    fprintf(output_file, "\n\t;le valor para %s\n", lexeme_of_id);
    fprintf(output_file, "\tmov edx,4\n");
    fprintf(output_file, "\tmov ecx,%s\n", lexeme_of_id);
    fprintf(output_file, "\tmov ebx,0\n");
    fprintf(output_file, "\tmov eax,3\n");
    fprintf(output_file, "\tint 0x80\n");
}

/**
 * @brief Gera codigo para o comando WRITE
 * 
 * @param lexeme_of_id nome do identificador
 */
void gen_write(char *lexeme_of_id) {
    /*fprintf(output_file, "mov rdi, fmtstr0\n");
    fprintf(output_file, "mov rsi, [rel %s]\n", lexeme_of_id);
    fprintf(output_file, "mov rax, 0\n");
    fprintf(output_file, "call printf\n");*/

    fprintf(output_file, "\n\t;escreve valor de %s\n", lexeme_of_id);
    fprintf(output_file, "\tmov edx,4\n");
    fprintf(output_file, "\tmov ecx,%s\n", lexeme_of_id);
    fprintf(output_file, "\tmov ebx,1\n");
    fprintf(output_file, "\tmov eax,4\n");
    fprintf(output_file, "\tint 0x80\n");
}