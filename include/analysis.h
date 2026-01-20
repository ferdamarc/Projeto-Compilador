#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_MODE 1                  /* Enable/disable debug mode */
#define MAXLEXEMA 21                  /* Maximum lexeme length */

#define ANSI_COLOR_RESET    "\e[0m"       /* Reset to default */
#define ANSI_COLOR_RED      "\e[0;31m"    /* Lexical errors */
#define ANSI_COLOR_YELLOW   "\e[0;33m"    /* Syntax errors */
#define ANSI_COLOR_PURPLE   "\e[0;35m"    /* Semantic errors */
#define ANSI_COLOR_WHITE    "\e[1;37m"    /* Line numbers */
#define ANSI_COLOR_GREEN    "\e[1;32m"    /* Error recovery */

/* Semantic Error Types */
typedef enum {
    DECL_VOID_VAR,          /* Variable declared as void */
    DECL_FUNC_EXISTS,       /* Function already declared */
    DECL_VAR_EXISTS,        /* Variable already declared */
    DECL_FUNC_VAR,          /* Function declared with existing variable name */
    DECL_VAR_FUNC,          /* Variable declared with existing function name */
    VAR_NOT_DECLARED,       /* Variable used before declaration */
    FUNC_NOT_DECLARED,      /* Function called before declaration */
    ASSIGN_FUNC_VOID,       /* Void function assigned to variable */
    FUNC_MAIN_NOT_DECLARED, /* Main function not declared */
    ARRAY_NOT_DECLARED,     /* Array used before declaration */
    FUNC_CALL_INVALID       /* Invalid function call (missing parentheses) */
} semantic_error_t;

/* AST Node Types */
typedef enum {
    NODE_DECLARATION,   /* Declaration node */
    NODE_EXPRESSION,    /* Expression node */
    NODE_NONE           /* Undefined node type */
} node_type_t;

/* Data Types */
typedef enum {
    TYPE_INT,       /* Integer type */
    TYPE_VOID       /* Void type (for functions) */
} data_type_t;

/* Declaration Types */
typedef enum {
    DECL_IF,            /* If statement */
    DECL_WHILE,         /* While loop */
    DECL_RETURN_INT,    /* Return statement with integer value */
    DECL_RETURN_VOID,   /* Return statement (void) */
    DECL_NULL,          /* Null declaration */
    DECL_VAR,           /* Variable declaration */
    DECL_ARRAY,         /* Array declaration */
    DECL_FUNC,          /* Function declaration */
    DECL_PARAM_VAR,     /* Variable parameter */
    DECL_PARAM_ARRAY,   /* Array parameter */
    DECL_PARAM_VOID     /* Void parameter */
} decl_type_t;

/* Expression Types */
typedef enum {
    EXPR_OP,            /* Operation (arithmetic) */
    EXPR_OP_REL,        /* Relational operator */
    EXPR_CONST,         /* Numeric constant */
    EXPR_ID,            /* Identifier (variable) */
    EXPR_CALL,          /* Function call (activation) */
    EXPR_ARRAY,         /* Array access */
    EXPR_ASSIGN,        /* Assignment */
    EXPR_NULL           /* Null expression */
} expr_type_t;


typedef struct ast_node {
    int line_num;                       /* Source code line number */
    node_type_t node_type;              /* Node category */
    decl_type_t decl_type;              /* Declaration type (if applicable) */
    expr_type_t expr_type;              /* Expression type (if applicable) */
    char lexeme[MAXLEXEMA];             /* Associated lexeme/identifier */
    struct ast_node *children[3];       /* Up to 3 child nodes */
    struct ast_node *sibling;           /* Sibling node */
} ast_node_t;

typedef ast_node_t* ast_node_ptr;


typedef struct line_node {
    int line_num;                       /* Line number in source */
    struct line_node *next;             /* Next node */
    struct line_node *prev;             /* Previous node */
} line_node_t;

typedef struct symbol_item {
    decl_type_t id_type;                /* Declaration type */
    data_type_t data_type;              /* Data type */
    char id_name[MAXLEXEMA];            /* Identifier name */
    char scope[MAXLEXEMA];              /* Scope */
    line_node_t *lines;                 /* Line list */
    struct symbol_item *next;           /* Next item */
    struct symbol_item *prev;           /* Previous item */
} symbol_item_t;

typedef symbol_item_t* symbol_item_ptr;
typedef line_node_t* line_node_ptr;


extern int flag_verbose;                  /* Verbose output flag */
extern int stack_index;                   /* Lexeme stack index */
extern int line_num;                      /* Line counter */
extern int lexical_errors;                /* Lexical error counter */
extern int syntax_errors;                 /* Syntax error counter */
extern int semantic_errors;               /* Semantic error counter */
extern char* yytext;                      /* Current lexeme text */
extern char lexeme_stack[4][MAXLEXEMA];   /* Lexeme stack */

/* File Handles */
extern FILE *input_file;                  /* Input source file */
extern FILE *copy_file;                   /* Copy of source file */
extern FILE *output_file;                 /* Analysis output file */
extern FILE *output_intermediate_file;    /* Intermediate code output */
extern FILE *output_assembly_file;        /* Assembly code output */


symbol_item_ptr* initialize_symbol_table();
void delete_symbol_table(symbol_item_ptr hash_table[]);
void print_symbol_table(symbol_item_ptr hash_table[]);
void insert_symbol(symbol_item_ptr hash_table[], decl_type_t id_type, data_type_t data_type, 
                   const char* id_name, const char* scope, int line);
void remove_symbol(symbol_item_ptr hash_table[], symbol_item_ptr item);
symbol_item_ptr search_symbol(symbol_item_ptr hash_table[], char id[], char scope[], 
                              decl_type_t id_type);
symbol_item_ptr search_symbol_expr(symbol_item_ptr hash_table[], char id[], char scope[], 
                                   expr_type_t id_type);
symbol_item_ptr search_symbol_any(symbol_item_ptr hash_table[], char id[], char scope[]);
symbol_item_ptr search_symbol_func(symbol_item_ptr hash_table[], char* lexeme);
symbol_item_ptr search_symbol_id(symbol_item_ptr hash_table[], char* id_name);
void add_line(symbol_item_ptr item, int line_value);
unsigned long hash(const char *str);


ast_node_ptr create_node(char lexeme[MAXLEXEMA], int line_num, node_type_t node_type, 
                         decl_type_t decl_type, expr_type_t expr_type);
ast_node_ptr add_sibling(ast_node_ptr root, ast_node_ptr node);
ast_node_ptr add_child(ast_node_ptr root, ast_node_ptr node);
ast_node_ptr new_node();
void show_tree(ast_node_ptr root, int level);
void free_tree(ast_node_ptr root);
enum yytokentype get_token(void);
ast_node_ptr parse(void);

void traverse_tree(ast_node_ptr syntax_tree, symbol_item_ptr* hash_table, char* scope);
void traverse_decl(ast_node_ptr syntax_tree, symbol_item_ptr* hash_table, char* aux_scope);
void traverse_expr(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], char scope[]);
int search_equal(symbol_item_ptr* hash_table, ast_node_ptr syntax_tree, int index, char* scope);
void semantic_error(semantic_error_t error, char name[], int line);
void show_semantic_error(semantic_error_t error, char* name, int line);

#endif