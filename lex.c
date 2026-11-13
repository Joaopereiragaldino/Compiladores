// Inclusao do cabecalho
#include "lex.h"

// Variaveis globais
int pos;
FILE *input_file;
//Definicao e inicializacao de estrutura
type_token key_words[] = {
    {IF, "if", NULL},
    {ELSE, "else", NULL},
    {WHILE, "while", NULL},
    {DO, "do", NULL},
    {READ, "read", NULL},
    {WRITE, "write", NULL},
    {INT, "int", NULL},
    {FLOAT, "float", NULL},
    {CHAR, "char", NULL},
    {STRING, "string", NULL},
    {BEGIN, "begin", NULL},
    {END, "end", NULL},
    {EQUAL, "==", NULL},
    {NE, "!=", NULL},
    {GE, ">=", NULL},
    {LE, "<=", NULL},
    {0, "", NULL}
};


/**
 * @brief Processo de inicializacao do lexico. Recebe o arquivo de entrada
 * contendo codigo e carrega-o em memoria.
 * 
 */
void initLex(char input_file_name[]) {
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        printf("[ERRO]\n\tArquivo de entrada não encontrado: %s\n\n", input_file_name);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Analisa o proximo comando no código de entrada e retorna estrutura de
 * dados com Token correspondente (ponteiro).
 * 
 * @return type_token* 
 */
type_token *getToken() {
    char buffer[MAX_CHAR];
    int pos_buffer;
    type_token *token;
    type_token *key_found;
    char ch;
    
    pos_buffer = 0;
    token = (type_token*) malloc(sizeof(type_token));
    strcpy(buffer, "");
    ch = fgetc(input_file);

    // Consome espacos, tabulacoes e quebras de linha
    while ( ch == ' ' || ch == '\t' || ch == '\n') {
        ch = fgetc(input_file);
    }

    // Verifica se NUMERO
    if ( isdigit(ch) ) {
        int num_float = 0; // flag para verificar se e um numero flutuante
        // constroi buffer com os digitos
        while ( isdigit(ch) || ch == '.') {
            if( ch == '.'){ // Se encontrar um ponto
                if(num_float == 1){ // Se encontrar "..", inválido
                    break;
                }
                num_float = 1; // Aciona a flag de um numero float
            }
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
        }
        ungetc(ch, input_file);
        buffer[pos_buffer] = '\0';
        if(num_float == 1){
            token->tag = NUM_FLOAT;
        }
        else
        { 
            token->tag = NUM_INT;
        }
        strcpy( token->lexema, buffer ); //copia buffer para lexema
    } 
    else if ( isalpha(ch) ) { //Verifica se entrada eh um alfa-numerico (palavra reservada ou identificador)
        buffer[pos_buffer++] = ch;
        ch = fgetc(input_file);
        while( isalnum(ch) ) {
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
        }
        ungetc(ch, input_file);
        buffer[pos_buffer] = '\0';
        key_found = keyWordFind(buffer);
        if (key_found != NULL) { //Palavra reservada
            token->tag = key_found->tag;
            strcpy(token->lexema, key_found->lexema);
            token->ptr_value = NULL;
            //o token sera retornado no final da funcao
        } else { //Identificador
            token->tag = ID;
            strcpy(token->lexema, buffer);
            token->ptr_value = NULL;
        }
    }
    //Verifica se PLUS (+)
    else if (ch == PLUS) {
        token->tag = PLUS;
        strcpy(token->lexema, "+");
        token->ptr_value = NULL;
    }
    //Verifica se MINUS (-)
    else if (ch == MINUS) {
        token->tag = MINUS;
        strcpy(token->lexema, "-");
        token->ptr_value = NULL;
    }
    //Verifica se MULT (*)
    else if (ch == MULT) {
        token->tag = MULT;
        strcpy(token->lexema, "*");
        token->ptr_value = NULL;
    }
    //Verifica se DIV (/)
    else if (ch == DIV) {
        token->tag = DIV;
        strcpy(token->lexema, "/");
        token->ptr_value = NULL;
    }
    //Verifica se OPEN_PAR -> "("
    else if (ch == OPEN_PAR) {
        token->tag = OPEN_PAR;
        strcpy(token->lexema, "(");
        token->ptr_value = NULL;
    }
    //Verifica se CLOSE_PAR -> ")"
    else if (ch == CLOSE_PAR) {
        token->tag = CLOSE_PAR;
        strcpy(token->lexema, ")");
        token->ptr_value = NULL;
    }
    //Verifica se SEMICOLON -> ';'
    else if (ch == SEMICOLON) {
        token->tag = SEMICOLON;
        strcpy(token->lexema, ";");
        token->ptr_value = NULL;
    }
    // Verifica se EQUAL ou ASSIGN -> '==' ou '='
    else if(ch == ASSIGN){
        // Primeiro trata o caso de '=='. O caso '=' é feito por exclusão
        ch = fgetc(input_file);
        if(ch == ASSIGN){
            token->tag = EQUAL;
            strcpy(token->lexema, "==");
        } else {
            ungetc(ch, input_file);
            token->tag = ASSIGN;
            strcpy(token->lexema, "=");
        }
        token->ptr_value = NULL;
    }
    // Verifique se operador -> ' " '
    else if (ch == DOUBLE_QUOTES){
        ch = fgetc(input_file); // Consome a primeira aspas

        // Le os carateres até encontrar a aspa de fechamento ou se estourar o buffer
        while((ch != DOUBLE_QUOTES) && (pos_buffer < MAX_CHAR - 1) && (ch != EOF)){
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
        }

        if (ch == DOUBLE_QUOTES){
        //Fecha a string
        buffer[pos_buffer] = ENDTOKEN; //insere \0 no final da string
        token->tag = STRING_VAL;
        strcpy(token->lexema, buffer);
        token->ptr_value = NULL;
        }else {
            // Erro léxico: o arquivo acabou e a string ficou aberta
            token->tag = ERROR;
            strcpy(token->lexema, "Erro: String nao fechada");
            token->ptr_value = NULL;
        }

    }
    // Verifica caracteres entre aspas simples (char literal)
    else if (ch == SINGLE_QUOTES) {
        ch = fgetc(input_file);
        if (ch != SINGLE_QUOTES && ch != EOF) {
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
            if (ch == SINGLE_QUOTES) {
                buffer[pos_buffer] = '\0';
                token->tag = CHAR_VAL;
                strcpy(token->lexema, buffer);
            } else {
                token->tag = ERROR;
                strcpy(token->lexema, "Erro: Char mal formatado");
                while(ch != SINGLE_QUOTES && ch != EOF) ch = fgetc(input_file);
            }
        } else {
            token->tag = ERROR;
            strcpy(token->lexema, "Erro: Char vazio");
        }
        token->ptr_value = NULL;
    }
    // Verifica operador Diferente (!=)
    else if (ch == '!') {
        ch = fgetc(input_file);
        if (ch == ASSIGN) {
            token->tag = NE;
            strcpy(token->lexema, "!=");
        } else {
            ungetc(ch, input_file);
            token->tag = ERROR;
            strcpy(token->lexema, "!");
        }
        token->ptr_value = NULL;
    }
    // Verifica LT ou LE (< ou <=)
    else if (ch == LT) {
        ch = fgetc(input_file);
        if (ch == ASSIGN) {
            token->tag = LE;
            strcpy(token->lexema, "<=");
        } else {
            ungetc(ch, input_file);
            token->tag = LT;
            strcpy(token->lexema, "<");
        }
        token->ptr_value = NULL;
    }
    // Verifica GT ou GE (> ou >=)
    else if (ch == GT) {
        ch = fgetc(input_file);
        if (ch == ASSIGN) {
            token->tag = GE;
            strcpy(token->lexema, ">=");
        } else {
            ungetc(ch, input_file);
            token->tag = GT;
            strcpy(token->lexema, ">");
        }
        token->ptr_value = NULL;
    }
    else if (ch == '}') {
        token->tag = CLOSE_BRACE;
        strcpy(token->lexema, "}");
        token->ptr_value = NULL;
    }
    else if (ch == '{') {
        token->tag = OPEN_BRACE;
        strcpy(token->lexema, "{");
        token->ptr_value = NULL;
    }
    // Verifica operador AND Lógico (&&)
    else if (ch == '&') {
        ch = fgetc(input_file);
        if (ch == '&') {
            token->tag = LG_AND;
            strcpy(token->lexema, "&&");
        } else {
            ungetc(ch, input_file);
            token->tag = ERROR;
            strcpy(token->lexema, "&");
        }
        token->ptr_value = NULL;
    }
    // Verifica operador OR Lógico (||)
    else if (ch == '|') {
        ch = fgetc(input_file);
        if (ch == '|') {
            token->tag = LG_OR;
            strcpy(token->lexema, "||");
        } else {
            ungetc(ch, input_file);
            token->tag = ERROR;
            strcpy(token->lexema, "|");
        }
        token->ptr_value = NULL;
    }
    else if (ch == COMMA){
        token->tag = COMMA;
        strcpy(token->lexema, ",");
        token->ptr_value = NULL;
    }
    //Verifica se FIM DE ARQUIVO (EOF)
    else if (ch == EOF) {
        token->tag = ENDTOKEN;
        strcpy(token->lexema, "\0");
    }
    //ERRO
    else {
        token->tag = ERROR;
        strcpy(token->lexema, "");
    }

    return token;
}

/**
 * @brief Busca na lista de palavras reservadas o parâmetro informado, retornando
 * o respectivo token, ou nulo caso nao exista.
 * 
 * @param char* palavra a ser verificada se corresponde a uma palavra reservada.
 * @return type_token* com o endereco de memoria do token correspondente, ou nulo.
 */
type_token *keyWordFind(char *word) {
    int i;
    i = 0;

    while( key_words[i].lexema[0] != '\0' ) {
        if ( strcmp(word, key_words[i].lexema) == 0 )
            return &key_words[i];
        i++;
    }
    return NULL;
}

/**
 * @brief Espia o proximo caractere util (ignora espacos) sem consumi-lo.
 *        Nao usa ftell/fseek (comportamento indefinido apos ungetc em modo texto).
 *        Le caracteres descartando espacos e devolve todos com ungetc.
 */
int peekChar(void) {
    int buf[64]; // buffer para chars lidos (limite seguro)
    int n = 0;
    int c;

    // Le chars ate encontrar um nao-espaco
    while (1) {
        c = fgetc(input_file);
        if (c == EOF) {
            break;
        }
        buf[n++] = c;
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            break;
        }
        if (n >= 63) break; // seguranca
    }

    // Devolve todos os chars lidos em ordem inversa
    while (n > 0) {
        ungetc(buf[--n], input_file);
    }

    return c; // o char nao-espaco encontrado (ou EOF)
}