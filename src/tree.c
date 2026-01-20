#include <strings.h>
#include "analysis.h"

// Cria um nó vazio para a árvore sintática
ast_node_ptr new_node() {
    ast_node_ptr node = (ast_node_ptr)malloc(sizeof(ast_node_t));

    node->children[0] = NULL;
    node->children[1] = NULL;
    node->children[2] = NULL;
    node->sibling = NULL;

    bzero(node->lexeme, MAXLEXEMA);
    node->line_num = 0;
    node->decl_type = DECL_NULL;
    node->node_type = NODE_NONE;
    node->expr_type = EXPR_NULL;

    return node;
}

// Cria um nó completo para a árvore sintática
ast_node_ptr create_node(char lexeme[MAXLEXEMA], int line_num, node_type_t node_type, 
                         decl_type_t decl_type, expr_type_t expr_type) {
    ast_node_ptr node = (ast_node_ptr)malloc(sizeof(ast_node_t));

    node->children[0] = NULL;
    node->children[1] = NULL;
    node->children[2] = NULL;
    node->sibling = NULL;

    strcpy(node->lexeme, lexeme);
    node->line_num = line_num;
    node->decl_type = decl_type;
    node->node_type = node_type;
    node->expr_type = expr_type;

    return node;
}

ast_node_ptr add_sibling(ast_node_ptr root, ast_node_ptr node) {
    if (root == NULL) return NULL;

    ast_node_ptr aux = root;
    while (aux->sibling != NULL) {
        aux = aux->sibling;
    }
    aux->sibling = node;

    return root;
}

ast_node_ptr add_child(ast_node_ptr root, ast_node_ptr node) {
    if (root == NULL) return NULL;

    int i;
    for (i = 0; i < 3 && root->children[i] != NULL; i++);
    root->children[i] = node;

    return root;
}

// Desaloca a árvore sintática de forma recursiva
void free_tree(ast_node_ptr root) {
    if (root == NULL) return;

    for (int i = 0; i < 3; i++) {
        free_tree(root->children[i]);
    }
    free_tree(root->sibling);
    free(root);
}

// Imprime a árvore sintática
void show_tree(ast_node_ptr root, int level) {
    if (root == NULL) return;

    for (int i = 0; i < level; i++) {
        fprintf(output_file, "\t");
    }
    fprintf(output_file, "%s\n", root->lexeme);
    
    for (int i = 0; i < 3; i++) {
        show_tree(root->children[i], level + 1);
    }
    show_tree(root->sibling, level);
}
