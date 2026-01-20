%{
#include <string.h>
#include "parser.h"
#include "analysis.h"

// Estrutura para palavras reservadas
typedef struct {
    const char* palavra;
    enum yytokentype token;
} PalavraReservada;

// Variáveis globais
int line_num;
char id[MAXLEXEMA];
int firstTime = 1;
int lexical_errors = 0;
int stack_index = -1;
char lexeme_stack[4][MAXLEXEMA];

// Protótipos
static enum yytokentype verificaPalavraReservada(const char* texto);
static void inicializaLexer(void);

%}

%option noyywrap

DIGIT       [0-9]
NUMBER      {DIGIT}+
LETRA       [a-zA-Z]
IDENT       {LETRA}({LETRA}|{DIGIT}|_)*
NEWLINE     \n
WHITESPACE  [ \t]+

%%

{WHITESPACE}    { /* Ignora espaços e tabulações */ }

"/*"    {
            // Comentário multi-linha
            char c, aux = 'x';
            do {
                c = input();
                if (c == EOF) break;
                if (c == '\n') line_num++;
                if (c == '*') aux = input();
            } while (aux != '/');
        }

{NEWLINE}   { line_num++; }

{NUMBER}    {
                if (strlen(yytext) >= MAXLEXEMA) {
                    printf(ANSI_COLOR_PURPLE "WARNING: " ANSI_COLOR_RESET);
                    printf("Numero muito grande, truncado para: ");
                    yytext[MAXLEXEMA-1] = '\0';
                    printf("%s\n", yytext);
                }
                stack_index++;
                strcpy(lexeme_stack[stack_index], yytext);
                return NUM;
            }

{IDENT}     {
                enum yytokentype token = verificaPalavraReservada(yytext);
                
                if (token == ID && strlen(yytext) >= MAXLEXEMA) {
                    //printf(ANSI_COLOR_PURPLE "WARNING: " ANSI_COLOR_RESET);
                    //printf("Identificador muito grande, truncado para: ");
                    yytext[MAXLEXEMA-1] = '\0';
                    //printf("'%s'\n", yytext);
                }
                
                if (token == ID) {
                    stack_index++;
                    bzero(lexeme_stack[stack_index], MAXLEXEMA);
                    strcpy(lexeme_stack[stack_index], yytext);
                }
                return token;
            }
        
"("     { return ABREPARENTESES; }
")"     { return FECHAPARENTESES; }
"["     { return ABRECOLCHETES; }
"]"     { return FECHACOLCHETES; }
"{"     { return ABRECHAVES; }
"}"     { return FECHACHAVES; }
"="     { return ATRIB; }
","     { return COMMA; }
";"     { return SEMICOLON; }
"+"     { return SOMA; }
"-"     { return SUB; }
"*"     { return MULT; }
"/"     { return DIV; }
"=="    { return EQ; }
"!="    { return NEQ; }
"<"     { return LT; }
">"     { return GT; }
"<="    { return LET; }
">="    { return GET; }

<<EOF>>     { return 0; }

.       {
            printf(ANSI_COLOR_RED "\nERRO LEXICO: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_WHITE "\"%s\" ", yytext);
            printf(ANSI_COLOR_RED "LINHA: " ANSI_COLOR_WHITE "%d" ANSI_COLOR_RESET "\n", line_num);
            lexical_errors++;
            return ERRO;
        }

%%

// ==================== FUNÇÕES AUXILIARES ====================

// Verifica se o texto é uma palavra reservada
static enum yytokentype verificaPalavraReservada(const char* texto) {
    static const PalavraReservada palavrasReservadas[] = {
        {"if",     IF},
        {"int",    INT},
        {"else",   ELSE},
        {"return", RETURN},
        {"void",   VOID},
        {"while",  WHILE},
        {NULL,     0}
    };

    for (int i = 0; palavrasReservadas[i].palavra != NULL; i++) {
        if (strcmp(texto, palavrasReservadas[i].palavra) == 0) {
            return palavrasReservadas[i].token;
        }
    }
    
    return ID;
}

// Copia o arquivo de entrada para arquivo temporário
static void copiar(FILE* arquivo) {
    FILE* copia = fopen("copia.txt", "w");
    if (copia == NULL) {
        printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
        printf("Nao foi possivel criar arquivo temporario.\n");
        exit(1);
    }

    char ch;
    while ((ch = fgetc(arquivo)) != EOF) {
        fputc(ch, copia);
    }

    fclose(copia);
}

// Inicializa o analisador léxico
static void inicializaLexer(void) {
    yyin = input_file;
    
    // Cria cópia do arquivo de entrada
    copiar(input_file);
    rewind(input_file);
    copy_file = fopen("copia.txt", "r");

    // Inicializa pilha de identificadores
    for (int i = 0; i < 4; i++) {
        bzero(lexeme_stack[i], MAXLEXEMA);
    }
}

// Retorna o próximo token
enum yytokentype get_token(void) {
    if (firstTime == 1) {
        firstTime = 0;
        inicializaLexer();
    }

    enum yytokentype currentToken = yylex();
    
    if (currentToken == ID) {
        strcpy(id, yytext);
    }
    
    return currentToken;
}

