%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "analysis.h"

#define YYSTYPE ast_node_ptr
#define MAX_NODES 10000

static int yylex(void);
void yyerror(char* s);
int yyparse(void);
void show_tree(ast_node_ptr root, int num);
enum yytokentype get_token(void);
ast_node_ptr parse(void);

enum yytokentype aux_error;
int syntax_errors = 0;

ast_node_ptr syntax_tree;       // Raiz da árvore sintática
ast_node_ptr nodes[MAX_NODES];  // Array de nós da árvore
int node_count = 0;             // Contador de nós
char aux_lexeme[MAXLEXEMA];     // Buffer auxiliar

%}

%token NUM SOMA SUB MULT DIV INT
%token ID VOID WHILE ELSE IF ABREPARENTESES FECHAPARENTESES
%token RETURN COMMA ABRECHAVES FECHACHAVES SEMICOLON
%token ATRIB ABRECOLCHETES FECHACOLCHETES
%token EQ NEQ LT LET GT GET ERRO

// Precedências para resolver o conflito shift/reduce do "dangling else"
%nonassoc IFX
%nonassoc ELSE

%%

// Programa principal
programa			: declaracao_lista {
                        syntax_tree = $1;
                    }
                    ;

// Lista de declarações
declaracao_lista	: declaracao_lista declaracao {
                        if ($1 != NULL) {
                            $$ = $1;
                            add_sibling($$, $2);
                        } else {
                            $$ = $2;
                        }
                    }
                    | declaracao { $$ = $1; }
                    ;

// Declaração (variável ou função)
declaracao			: var_declaracao { $$ = $1; }
                    | fun_declaracao { $$ = $1; }
                    ;

// Declaração de variável simples ou vetor
var_declaracao		: tipo_especificador ID SEMICOLON {
                        $$ = $1;
                        $$->node_type = NODE_DECLARATION;
                        $$->decl_type = DECL_VAR;
                        $$->line_num = line_num;
                    
                        ast_node_ptr aux = new_node();
                        strcpy(aux->lexeme, lexeme_stack[stack_index]);
                        stack_index--;
                        add_child($$, aux);

                        nodes[node_count++] = aux;
                    }
                    | tipo_especificador error SEMICOLON {
                        yyerrok;
                        printf(ANSI_COLOR_GREEN "RECUPERAÇÃO DE ERRO: " ANSI_COLOR_RESET);
                        printf("Ignorando declaração inválida\n");
                        $$ = NULL;
                    }
                    | tipo_especificador ID ABRECOLCHETES NUM FECHACOLCHETES SEMICOLON {
                        $$ = $1;
                        $$->node_type = NODE_DECLARATION;
                        $$->decl_type = DECL_ARRAY;
                        $$->line_num = line_num;

                        ast_node_ptr aux = new_node();
                        ast_node_ptr aux2 = new_node();
                        
                        strcpy(aux->lexeme, lexeme_stack[stack_index--]);
                        strcpy(aux2->lexeme, lexeme_stack[stack_index--]);
                        
                        add_child($$, aux2);
                        add_child($$, aux);

                        nodes[node_count++] = aux;
                        nodes[node_count++] = aux2;
                    }
                    | tipo_especificador error FECHACOLCHETES SEMICOLON {
                        yyerrok;
                        printf(ANSI_COLOR_GREEN "RECUPERAÇÃO DE ERRO: " ANSI_COLOR_RESET);
                        printf("Ignorando declaração inválida\n");
                        $$ = NULL;
                    }
                    ;

// Tipo de dado (int ou void)
tipo_especificador 	: INT {
                        $$ = new_node();
                        strcpy($$->lexeme, "INT");
                        $$->line_num = line_num;
                        nodes[node_count++] = $$;
                    }
                    | VOID {
                        $$ = new_node();
                        strcpy($$->lexeme, "VOID");
                        $$->line_num = line_num;
                        nodes[node_count++] = $$;
                    }
                    ;

// Declaração de função
fun_declaracao		: tipo_especificador fun_id ABREPARENTESES params FECHAPARENTESES composto_decl {
                        $$ = $1;
                        add_child($$, $4);
                        add_child($$, $2);
                        add_child($2, $6);
                        $$->node_type = NODE_DECLARATION;
                        $$->decl_type = DECL_FUNC;
                    }
                    ;

// Identificador de função
fun_id				: ID {
                        $$ = new_node();
                        strcpy($$->lexeme, lexeme_stack[stack_index--]);
                        $$->line_num = line_num;
                        nodes[node_count++] = $$;
                    }
                    ;	

// Parâmetros de função
params				: param_lista { $$ = $1; }
                    | VOID {
                        $$ = new_node();
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_PARAM_VOID;
                        strcpy($$->lexeme, "VOID");
                        nodes[node_count++] = $$;
                    }
                    ;

param_lista			: param_lista COMMA param {
                        if ($1 != NULL) {
                            $$ = $1;
                            add_sibling($$, $3);
                        } else {
                            $$ = $3;
                        }
                    }
                    | param { $$ = $1; }
                    ;

param				: tipo_especificador ID {
                        $$ = $1;
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_PARAM_VAR;

                        ast_node_ptr aux = new_node();
                        strcpy(aux->lexeme, lexeme_stack[stack_index--]);
                        add_child($$, aux);
                        nodes[node_count++] = aux;
                    }
                    | tipo_especificador ID ABRECOLCHETES FECHACOLCHETES {
                        $$ = $1;
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_PARAM_ARRAY;

                        ast_node_ptr aux = new_node();
                        strcpy(aux->lexeme, lexeme_stack[stack_index--]);
                        add_child($$, aux);
                        nodes[node_count++] = aux;
                    }
                    ;

// Bloco composto (escopo)
composto_decl		: ABRECHAVES local_declaracoes statement_lista FECHACHAVES {
                        if ($2 != NULL) {
                            $$ = $2;
                            add_sibling($$, $3);
                        } else {
                            $$ = $3;
                        }
                    }
                    ;

local_declaracoes 	: local_declaracoes var_declaracao {
                        if ($1 != NULL) {
                            $$ = $1;
                            add_sibling($$, $2);
                        } else {
                            $$ = $2;
                        }
                    }
                    | %empty { $$ = NULL; }
                    ;

statement_lista 	: statement_lista statement {
                        if ($1 != NULL) {
                            $$ = $1;
                            add_sibling($$, $2);
                        } else {
                            $$ = $2;
                        }
                    }
                    | %empty { $$ = NULL; }
                    ;

// Comandos
statement			: expressao_decl { $$ = $1; }
                    | composto_decl { $$ = $1; }
                    | selecao_decl { $$ = $1; }
                    | iteracao_decl { $$ = $1; }
                    | retorno_decl { $$ = $1; }
                    ;

// Declaração de expressão
expressao_decl		: expressao SEMICOLON { $$ = $1; }
                    | SEMICOLON { $$ = NULL; }
                    | error SEMICOLON {
                        yyerrok;
                        printf(ANSI_COLOR_GREEN "RECUPERAÇÃO DE ERRO: " ANSI_COLOR_RESET);
                        printf("Sincronizando em ';'\n");
                        $$ = NULL;
                    }
                    ;

// Estrutura condicional (if/else)
selecao_decl		: IF ABREPARENTESES expressao FECHAPARENTESES statement %prec IFX {
                        $$ = new_node();
                        strcpy($$->lexeme, "IF");
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_IF;
                        add_child($$, $3);
                        add_child($$, $5);
                        nodes[node_count++] = $$;
                    }
                    | IF ABREPARENTESES expressao FECHAPARENTESES statement ELSE statement {
                        $$ = new_node();
                        strcpy($$->lexeme, "IF");
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_IF;
                        add_child($$, $3);
                        add_child($$, $5);
                        add_child($$, $7);
                        nodes[node_count++] = $$;
                    }
                    ;

// Estrutura de repetição (while)
iteracao_decl		: WHILE ABREPARENTESES expressao FECHAPARENTESES statement {
                        $$ = new_node();
                        strcpy($$->lexeme, "WHILE");
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_WHILE;
                        add_child($$, $3);
                        add_child($$, $5);
                        nodes[node_count++] = $$;
                    }
                    ;

// Retorno de função
retorno_decl		: RETURN SEMICOLON {
                        $$ = new_node();
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_RETURN_VOID;
                        strcpy($$->lexeme, "ReturnVOID");
                        nodes[node_count++] = $$;
                    }
                    | RETURN expressao SEMICOLON {
                        $$ = new_node();
                        $$->node_type = NODE_DECLARATION;
                        $$->line_num = line_num;
                        $$->decl_type = DECL_RETURN_INT;
                        strcpy($$->lexeme, "ReturnINT");
                        add_child($$, $2);
                        nodes[node_count++] = $$;
                    }
                    ;

// Expressão (atribuição ou expressão simples)
expressao			: var ATRIB expressao {
                        $$ = new_node();
                        strcpy($$->lexeme, "=");
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_ASSIGN;
                        add_child($$, $1);
                        add_child($$, $3);
                        nodes[node_count++] = $$;
                    }
                    | simples_expressao { $$ = $1; }
                    ;

// Variável (simples ou vetor)
var 				: ID {
                        $$ = new_node();
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_ID;
                        strcpy($$->lexeme, lexeme_stack[stack_index--]);
                        nodes[node_count++] = $$;
                    }
                    | ID ABRECOLCHETES expressao FECHACOLCHETES {
                        $$ = new_node();
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_ARRAY;
                        strcpy($$->lexeme, lexeme_stack[stack_index--]);
                        add_child($$, $3);
                        nodes[node_count++] = $$;
                    }
                    ;
            
// Expressão simples (com ou sem operador relacional)
simples_expressao	: soma_expressao relacional soma_expressao {
                        $$ = $2;
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_OP_REL;
                        add_child($$, $1);
                        add_child($$, $3);
                    }
                    | soma_expressao { $$ = $1; }
                    ;

relacional			: operador_relacional { $$ = $1; }
                    ;

// Operadores relacionais
operador_relacional	: EQ  { $$ = new_node(); strcpy($$->lexeme, "=="); nodes[node_count++] = $$; }
                    | NEQ { $$ = new_node(); strcpy($$->lexeme, "!="); nodes[node_count++] = $$; }
                    | LT  { $$ = new_node(); strcpy($$->lexeme, "<");  nodes[node_count++] = $$; }
                    | GT  { $$ = new_node(); strcpy($$->lexeme, ">");  nodes[node_count++] = $$; }
                    | LET { $$ = new_node(); strcpy($$->lexeme, "<="); nodes[node_count++] = $$; }
                    | GET { $$ = new_node(); strcpy($$->lexeme, ">="); nodes[node_count++] = $$; }
                    ;

// Expressão de soma/subtração
soma_expressao		: soma_expressao soma termo {
                        $$ = $2;
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_OP;
                        add_child($$, $1);
                        add_child($$, $3);
                    }
                    | termo { $$ = $1; }
                    ;

// Operadores de adição
soma				: SOMA { $$ = new_node(); strcpy($$->lexeme, "+"); nodes[node_count++] = $$; }
                    | SUB  { $$ = new_node(); strcpy($$->lexeme, "-"); nodes[node_count++] = $$; }
                    ;

// Termo (multiplicação/divisão)
termo				: termo mult fator {
                        $$ = $2;
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_OP;
                        add_child($$, $1);
                        add_child($$, $3);
                    }
                    | fator { $$ = $1; }
                    ;

// Operadores de multiplicação
mult				: MULT { $$ = new_node(); strcpy($$->lexeme, "*"); nodes[node_count++] = $$; }
                    | DIV  { $$ = new_node(); strcpy($$->lexeme, "/"); nodes[node_count++] = $$; }
                    ;

// Fator (expressão básica)
fator				: ABREPARENTESES expressao FECHAPARENTESES { $$ = $2; }
                    | var { $$ = $1; }
                    | ativacao { $$ = $1; }
                    | NUM {
                        $$ = new_node();
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_CONST;
                        strcpy($$->lexeme, lexeme_stack[stack_index--]);
                        nodes[node_count++] = $$;
                    }
                    ;

// Chamada de função
ativacao 			: fun_id ABREPARENTESES args FECHAPARENTESES {
                        $$ = $1;
                        $$->node_type = NODE_EXPRESSION;
                        $$->line_num = line_num;
                        $$->expr_type = EXPR_CALL;
                        add_child($$, $3);
                    }
                    ;

// Argumentos de função
args 				: arg_lista { $$ = $1; }
                    | %empty { $$ = NULL; }
                    ;

arg_lista			: arg_lista COMMA expressao {
                        if ($1 != NULL) {
                            $$ = $1;
                            add_sibling($$, $3);
                        } else {
                            $$ = $3;
                        }
                    }
                    | expressao { $$ = $1; }
                    ;	

%%

/* Tratamento de erros sintáticos */
/* Tentativa simples de tentar mapear vários erros da análise sintática;
 * A utilização do yyerrok faz-se possível para conseguir mapear os erros.
*/
void yyerror(char *s) {
    printf(ANSI_COLOR_YELLOW "ERRO SINTÁTICO: " ANSI_COLOR_RESET);
    printf(ANSI_COLOR_WHITE "\"%s\" ", yytext);
    printf(ANSI_COLOR_YELLOW "LINHA: " ANSI_COLOR_WHITE "%d" ANSI_COLOR_RESET, line_num);
    printf(" | %s\n", s);
    syntax_errors++;
}

// Única interface com o analisador léxico
int yylex(void) {
    return (aux_error = get_token());
}

// Função principal de parsing
ast_node_ptr parse(void) {
    yyparse();
    return syntax_tree;
}

