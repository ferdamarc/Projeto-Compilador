#include "analysis.h"

void traverse_tree(ast_node_ptr syntax_tree, symbol_item_ptr* hash_table, char* scope) {
    if (syntax_tree == NULL) {
        return;
    }

    char aux_scope[MAXLEXEMA];
    strcpy(aux_scope, scope);

    // Processa declarações
    if (syntax_tree->node_type == NODE_DECLARATION) {
        traverse_decl(syntax_tree, hash_table, aux_scope);
    }

    // Processa expressões
    if (syntax_tree->node_type == NODE_EXPRESSION) {
        traverse_expr(syntax_tree, hash_table, aux_scope);
    }

    // Percorre os filhos
    if (syntax_tree->expr_type != EXPR_CALL) {
        for (int i = 0; i < 3; i++) {
            if (syntax_tree->children[i] != NULL) {
                traverse_tree(syntax_tree->children[i], hash_table, aux_scope);
            }
        }
    }

    // Percorre irmãos
    if (strcmp(scope, "global") == 0) {
        traverse_tree(syntax_tree->sibling, hash_table, scope);
    } else {
        traverse_tree(syntax_tree->sibling, hash_table, aux_scope);
    }
}

static void validate_parameter(symbol_item_ptr* hash_table, ast_node_ptr parameter, 
                               char* scope, int func_line) {
    // Verifica se o parâmetro já foi declarado
    if (search_equal(hash_table, parameter, 0, scope) != 1) {
        return;  // Parâmetro já existe, erro já reportado
    }
    
    // Verifica se o parâmetro é do tipo inteiro
    if (strcmp(parameter->lexeme, "INT") == 0) {
        insert_symbol(hash_table, parameter->decl_type, TYPE_INT, 
                     parameter->children[0]->lexeme, scope, func_line);
    } else {
        show_semantic_error(DECL_VOID_VAR, parameter->children[0]->lexeme, parameter->line_num);
    }
}

static void process_function_decl(symbol_item_ptr* hash_table, ast_node_ptr syntax_tree, 
                                  char* aux_scope) {
    // Determina o tipo de retorno da função
    data_type_t return_type = (strcmp(syntax_tree->lexeme, "INT") == 0) ? TYPE_INT : TYPE_VOID;
    
    // O escopo da função é seu próprio nome
    strcpy(aux_scope, syntax_tree->children[1]->lexeme);
    
    // Verifica se a função já foi declarada
    if (search_equal(hash_table, syntax_tree, 1, aux_scope) == 1) {
        insert_symbol(hash_table, syntax_tree->decl_type, return_type, 
                     syntax_tree->children[1]->lexeme, aux_scope, syntax_tree->line_num);
    }

    // Processa parâmetros da função (se houver)
    if (syntax_tree->children[0]->decl_type != DECL_PARAM_VOID) {
        ast_node_ptr current_param = syntax_tree->children[0];
        
        while (current_param != NULL) {
            validate_parameter(hash_table, current_param, aux_scope, syntax_tree->line_num);
            current_param = current_param->sibling;
        }
    }
}

static void process_variable_decl(symbol_item_ptr* hash_table, ast_node_ptr syntax_tree, 
                                  char* scope) {
    // Verifica se a variável/vetor já foi declarado
    if (search_equal(hash_table, syntax_tree, 0, scope) != 1) {
        return;  // Já existe, erro já reportado
    }
    
    // Verifica se é do tipo inteiro (variáveis não podem ser void)
    if (strcmp(syntax_tree->lexeme, "INT") == 0) {
        insert_symbol(hash_table, syntax_tree->decl_type, TYPE_INT, 
                     syntax_tree->children[0]->lexeme, scope, syntax_tree->line_num);
    } else {
        show_semantic_error(DECL_VOID_VAR, syntax_tree->children[0]->lexeme, 
                           syntax_tree->line_num);
    }
}

void traverse_decl(ast_node_ptr syntax_tree, symbol_item_ptr* hash_table, char* aux_scope) {
    if (syntax_tree == NULL) {
        return;
    }

    // Processa conforme o tipo de declaração
    if (syntax_tree->decl_type == DECL_FUNC) {
        process_function_decl(hash_table, syntax_tree, aux_scope);
    } 
    else if (syntax_tree->decl_type == DECL_VAR || 
             syntax_tree->decl_type == DECL_ARRAY) {
        process_variable_decl(hash_table, syntax_tree, aux_scope);
    }
}

static void func_call(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], char scope[]) {
    if (syntax_tree == NULL) {
        return;
    }

    symbol_item_ptr found_item = NULL;

    // Caso 1: Chamada de função
    if (syntax_tree->expr_type == EXPR_CALL) {
        found_item = search_symbol_expr(hash_table, syntax_tree->lexeme, 
                                        scope, syntax_tree->expr_type);
        
        if (found_item == NULL) {
            show_semantic_error(FUNC_NOT_DECLARED, syntax_tree->lexeme, 
                               syntax_tree->line_num);
        } else {
            add_line(found_item, syntax_tree->line_num);
        }
        return;
    }
    
    // Caso 2: Uso de identificador (variável)
    if (syntax_tree->expr_type == EXPR_ID) {
        unsigned int index = hash(syntax_tree->lexeme);
        symbol_item_ptr current_item = hash_table[index];

        // Percorre a lista procurando o identificador
        while (current_item != NULL) {
            if (strcmp(current_item->id_name, syntax_tree->lexeme) == 0) {
                // Verifica se é uma função (erro: chamada sem parênteses)
                if (current_item->id_type == DECL_FUNC) {
                    show_semantic_error(FUNC_CALL_INVALID, syntax_tree->lexeme, 
                                       syntax_tree->line_num);
                    return;
                }
                
                // Verifica se está no escopo correto
                if (strcmp(current_item->scope, scope) == 0 || 
                    strcmp(current_item->scope, "global") == 0) {
                    add_line(current_item, syntax_tree->line_num);
                    return;
                }
            }
            current_item = current_item->next;
        }

        // Variável não encontrada
        show_semantic_error(VAR_NOT_DECLARED, syntax_tree->lexeme, 
                           syntax_tree->line_num);
    }
}

static void validate_identifier(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], char scope[]) {
    symbol_item_ptr found_item = search_symbol_expr(hash_table, syntax_tree->lexeme, 
                                                     scope, syntax_tree->expr_type);
    
    if (found_item == NULL) {
        // Verifica se não é uma função sendo usada sem parênteses
        if (search_symbol_expr(hash_table, syntax_tree->lexeme, scope, EXPR_CALL) != NULL) {
            show_semantic_error(FUNC_CALL_INVALID, syntax_tree->lexeme, syntax_tree->line_num);
        } else {
            show_semantic_error(VAR_NOT_DECLARED, syntax_tree->lexeme, syntax_tree->line_num);
        }
    } else {
        add_line(found_item, syntax_tree->line_num);
    }
}

static void validate_function_call(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], char scope[]) {
    symbol_item_ptr found_item = search_symbol_expr(hash_table, syntax_tree->lexeme, 
                                                     scope, syntax_tree->expr_type);
    
    if (found_item == NULL) {
        show_semantic_error(FUNC_NOT_DECLARED, syntax_tree->lexeme, syntax_tree->line_num);
    } else {
        add_line(found_item, syntax_tree->line_num);
    }
    
    // Valida os argumentos da função
    ast_node_ptr current_arg = syntax_tree->children[0];
    while (current_arg != NULL) {
        if (current_arg->children[0] != NULL) {
            // Argumento complexo (expressão)
            traverse_tree(current_arg, hash_table, scope);
        } else {
            // Argumento simples (identificador ou chamada)
            func_call(current_arg, hash_table, scope);
        }
        current_arg = current_arg->sibling;
    }
}

static void validate_assignment(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], char scope[]) {
    // Verifica se está atribuindo uma chamada de função
    if (syntax_tree->children[1]->expr_type != EXPR_CALL) {
        return;
    }
    
    symbol_item_ptr func_item = search_symbol_expr(hash_table, syntax_tree->children[1]->lexeme, 
                                                    scope, syntax_tree->children[1]->expr_type);
    
    // Se a função não foi encontrada, o erro será reportado em outra validação
    if (func_item == NULL) {
        return;
    }
    
    // Verifica se a função é do tipo void
    if (func_item->data_type == TYPE_VOID) {
        show_semantic_error(ASSIGN_FUNC_VOID, syntax_tree->children[1]->lexeme, 
                           syntax_tree->children[1]->line_num);
    }
}

static void validate_array(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], char scope[]) {
    symbol_item_ptr found_item = search_symbol_expr(hash_table, syntax_tree->lexeme, 
                                                     scope, syntax_tree->expr_type);
    
    if (found_item == NULL) {
        show_semantic_error(ARRAY_NOT_DECLARED, syntax_tree->lexeme, syntax_tree->line_num);
    } else {
        add_line(found_item, syntax_tree->line_num);
    }
}

void traverse_expr(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], char scope[]) {
    if (syntax_tree == NULL) {
        return;
    }

    expr_type_t expr_type = syntax_tree->expr_type;

    switch (expr_type) {
        case EXPR_ID:
            validate_identifier(syntax_tree, hash_table, scope);
            break;
            
        case EXPR_CALL:
            validate_function_call(syntax_tree, hash_table, scope);
            break;
            
        case EXPR_ASSIGN:
            validate_assignment(syntax_tree, hash_table, scope);
            break;
            
        case EXPR_ARRAY:
            validate_array(syntax_tree, hash_table, scope);
            break;
            
        default:
            // Outros tipos de expressão não requerem validação adicional
            break;
    }
}

static int is_variable_type(decl_type_t type) {
    return (type == DECL_VAR || type == DECL_ARRAY || 
            type == DECL_PARAM_VAR || type == DECL_PARAM_ARRAY);
}


int search_equal(symbol_item_ptr* hash_table, ast_node_ptr syntax_tree, int index, char* scope) {
    if (syntax_tree == NULL || syntax_tree->children[index] == NULL) {
        return 1;
    }
    
    // Busca símbolo com mesmo nome na tabela
    symbol_item_ptr found_item = search_symbol(hash_table, 
                                               syntax_tree->children[index]->lexeme, 
                                               scope, 
                                               syntax_tree->decl_type);
    
    // Se não encontrou, não há conflito
    if (found_item == NULL) {
        return 1;
    }
    
    decl_type_t existing_type = found_item->id_type;
    decl_type_t new_type = syntax_tree->decl_type;
    char* id_name = syntax_tree->children[index]->lexeme;
    int line = syntax_tree->line_num;

    // Caso 1: Tentativa de redeclarar função
    if (existing_type == DECL_FUNC && new_type == DECL_FUNC) {
        show_semantic_error(DECL_FUNC_EXISTS, id_name, line);
        return 0;
    }
    
    // Caso 2: Tentativa de redeclarar variável/vetor no mesmo escopo
    if (is_variable_type(existing_type) && is_variable_type(new_type)) {
        // Permite variáveis com mesmo nome em escopos diferentes
        if (strcmp(found_item->scope, scope) != 0 && 
            strcmp(found_item->scope, "global") != 0) {
            return 1;  // Escopos diferentes, pode declarar
        }
        
        show_semantic_error(DECL_VAR_EXISTS, id_name, line);
        return 0;
    }
    
    // Caso 3: Tentativa de declarar função com nome de variável existente
    if (is_variable_type(existing_type) && new_type == DECL_FUNC) {
        show_semantic_error(DECL_FUNC_VAR, id_name, line);
        return 0;
    }
    
    // Caso 4: Tentativa de declarar variável com nome de função existente
    if (existing_type == DECL_FUNC && is_variable_type(new_type)) {
        show_semantic_error(DECL_VAR_FUNC, id_name, line);
        return 0;
    }

    return 0;
}


static const char* get_error_message(semantic_error_t error, const char* name) {
    static char buffer[256];
    
    switch (error) {
        case DECL_VOID_VAR:
            snprintf(buffer, sizeof(buffer), 
                    ": Variavel '%s' declarada como void", name);
            break;
            
        case DECL_FUNC_EXISTS:
            snprintf(buffer, sizeof(buffer), 
                    ": Funcao '%s' ja declarada", name);
            break;
            
        case DECL_VAR_EXISTS:
            snprintf(buffer, sizeof(buffer), 
                    ": Variavel '%s' ja declarada", name);
            break;
            
        case DECL_FUNC_VAR:
            snprintf(buffer, sizeof(buffer), 
                    ": Identificador '%s' ja declarado como variavel", name);
            break;
            
        case DECL_VAR_FUNC:
            snprintf(buffer, sizeof(buffer), 
                    ": Identificador '%s' ja declarado como funcao", name);
            break;
            
        case VAR_NOT_DECLARED:
            snprintf(buffer, sizeof(buffer), 
                    ": Variavel '%s' nao declarada", name);
            break;
            
        case FUNC_NOT_DECLARED:
            snprintf(buffer, sizeof(buffer), 
                    ": Funcao '%s' nao declarada", name);
            break;
            
        case ASSIGN_FUNC_VOID:
            snprintf(buffer, sizeof(buffer), 
                    ": Atribuicao invalida: funcao '%s' do tipo void", name);
            break;
            
        case FUNC_MAIN_NOT_DECLARED:
            snprintf(buffer, sizeof(buffer), 
                    ": Funcao main nao declarada");
            break;
            
        case ARRAY_NOT_DECLARED:
            snprintf(buffer, sizeof(buffer), 
                    ": Vetor '%s' nao declarado", name);
            break;
            
        case FUNC_CALL_INVALID:
            snprintf(buffer, sizeof(buffer), 
                    ": Chamada de funcao '%s' invalida, utilizar os ()", name);
            break;
            
        default:
            snprintf(buffer, sizeof(buffer), 
                    ": Erro semantico desconhecido");
            break;
    }
    
    return buffer;
}

int semantic_errors = 0;

/* Método criado para todas os erros semânticos */
void show_semantic_error(semantic_error_t error, char* name, int line) {
    semantic_errors++;
    
    // Exibe cabeçalho do erro com destaque
    printf(ANSI_COLOR_PURPLE "ERRO SEMANTICO, LINHA: %d" ANSI_COLOR_RESET, line);
    
    // Exibe mensagem específica do erro
    const char* message = get_error_message(error, name);
    printf("%s\n\n", message);
}