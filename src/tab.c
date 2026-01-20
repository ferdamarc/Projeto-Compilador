#include "analysis.h"

#define HASH_TABLE_SIZE 211
#define HASH_ALPHA_SHIFT 4

// Protótipos de funções internas
unsigned long hash(const char *str);

symbol_item_ptr* initialize_symbol_table() {
    symbol_item_ptr* hash_table = (symbol_item_ptr*)malloc(HASH_TABLE_SIZE * sizeof(symbol_item_ptr));
    
    if (hash_table == NULL) {
        fprintf(stderr, "ERRO FATAL: Falha ao alocar memória para tabela de símbolos\n");
        return NULL;
    }
    
    // Inicializa todos os buckets como vazios
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_table[i] = NULL;
    }
    
    return hash_table;
}


symbol_item_ptr search_symbol_id(symbol_item_ptr hash_table[], char* id_name) {
    unsigned int index = hash(id_name);
    symbol_item_ptr current_item = hash_table[index];

    // Percorre a lista encadeada no bucket
    while (current_item != NULL) {
        if (strcmp(current_item->id_name, id_name) == 0) {
            return current_item;
        }
        current_item = current_item->next;
    }
    
    return NULL;
}


symbol_item_ptr search_symbol_func(symbol_item_ptr hash_table[], char* lexeme) {
    unsigned int index = hash(lexeme);
    symbol_item_ptr current_item = hash_table[index];

    // Percorre a lista procurando especificamente uma função
    while (current_item != NULL) {
        if (current_item->id_type == DECL_FUNC && 
            strcmp(current_item->id_name, lexeme) == 0) {
            return current_item;
        }
        current_item = current_item->next;
    }
    
    return NULL;
}

/* Inserção e remocao de itens  */
static symbol_item_ptr create_new_item(decl_type_t id_type, data_type_t data_type,
                                       const char* id_name, const char* scope, int line) {
    symbol_item_ptr new_item = (symbol_item_ptr)malloc(sizeof(symbol_item_t));
    
    if (new_item == NULL) {
        fprintf(stderr, "ERRO: Falha ao alocar memória para novo símbolo '%s'\n", id_name);
        return NULL;
    }
    
    // Inicializa os campos do item
    new_item->id_type = id_type;
    new_item->data_type = data_type;
    strcpy(new_item->id_name, id_name);
    strcpy(new_item->scope, scope);
    new_item->lines = NULL;
    new_item->next = NULL;
    new_item->prev = NULL;
    
    // Adiciona a primeira linha de ocorrência
    add_line(new_item, line);
    
    return new_item;
}


void insert_symbol(symbol_item_ptr hash_table[], decl_type_t id_type, data_type_t data_type, 
                   const char* id_name, const char* scope, int line) {
    unsigned int index = hash(id_name);
    
    // Caso 1: Bucket vazio - primeiro item neste índice
    if (hash_table[index] == NULL) {
        symbol_item_ptr new_item = create_new_item(id_type, data_type, 
                                                    id_name, scope, line);
        if (new_item != NULL) {
            hash_table[index] = new_item;
        }
        return;
    }
    
    // Caso 2: Bucket ocupado - procura símbolo existente ou insere no final
    symbol_item_ptr current_item = hash_table[index];
    symbol_item_ptr prev_item = NULL;
    
    while (current_item != NULL) {
        // Verifica se é o mesmo símbolo no mesmo escopo (ou global)
        if (strcmp(current_item->id_name, id_name) == 0 && 
            (strcmp(current_item->scope, scope) == 0 || strcmp(current_item->scope, "global") == 0)) {
            // Símbolo já existe - apenas adiciona nova linha
            add_line(current_item, line);
            return;
        }
        
        prev_item = current_item;
        current_item = current_item->next;
    }
    
    // Símbolo não existe - insere no final da lista
    symbol_item_ptr new_item = create_new_item(id_type, data_type, 
                                                id_name, scope, line);
    if (new_item != NULL && prev_item != NULL) {
        prev_item->next = new_item;
        new_item->prev = prev_item;
    }
}

void remove_symbol(symbol_item_ptr hash_table[], symbol_item_ptr item) {
    if (item == NULL) {
        return;
    }
    
    unsigned int index = hash(item->id_name);

    // Caso 1: Item é o primeiro
    if (hash_table[index] == item) {
        hash_table[index] = item->next;
        if (item->next != NULL) {
            item->next->prev = NULL;
        }
    } 
    // Caso 2: Item está no meio ou fim da lista
    else {
        if (item->prev != NULL) {
            item->prev->next = item->next;
        }
        if (item->next != NULL) {
            item->next->prev = item->prev;
        }
    }
    
    // Libera a memória do item
    free(item);
}

/* === Parte da Análise semântica === */
symbol_item_ptr search_symbol_expr(symbol_item_ptr hash_table[], char id[], 
                                   char scope[], expr_type_t id_type) {
    unsigned int index = hash(id);
    symbol_item_ptr current_item = hash_table[index];

    // Caso especial: Chamada de função
    if (id_type == EXPR_CALL) {
        while (current_item != NULL) {
            if (strcmp(id, current_item->id_name) == 0 && 
                current_item->id_type == DECL_FUNC) {
                return current_item;
            }
            current_item = current_item->next;
        }
        return NULL;  // Função não encontrada
    }

    // Determina o tipo de declaração esperado baseado no uso
    decl_type_t expected_type = (id_type == EXPR_ARRAY) ? DECL_ARRAY : DECL_VAR;

    // Busca símbolo com o tipo correto no escopo apropriado
    while (current_item != NULL) {
        if (strcmp(id, current_item->id_name) == 0) {
            // Verifica se o tipo de declaração corresponde ao uso
            int type_correct = 0;
            
            if (expected_type == DECL_ARRAY) {
                // Aceita vetor declarado ou parâmetro vetor
                type_correct = (current_item->id_type == DECL_ARRAY || 
                               current_item->id_type == DECL_PARAM_ARRAY);
            } else {
                // Aceita variável declarada ou parâmetro variável
                type_correct = (current_item->id_type == DECL_VAR || 
                               current_item->id_type == DECL_PARAM_VAR);
            }
            
            // Verifica se está no escopo correto (local ou global)
            if (type_correct && 
                (strcmp(scope, current_item->scope) == 0 || 
                 strcmp(current_item->scope, "global") == 0)) {
                return current_item;
            }
        }
        current_item = current_item->next;
    }

    return NULL;  // Símbolo não encontrado ou tipo incorreto
}


symbol_item_ptr search_symbol(symbol_item_ptr hash_table[], char id[], 
                              char scope[], decl_type_t id_type) {
    unsigned int index = hash(id);
    symbol_item_ptr current_item = hash_table[index];

    // Caso especial: Declaração de função (busca global)
    if (id_type == DECL_FUNC) {
        while (current_item != NULL) {
            if (strcmp(id, current_item->id_name) == 0) {
                return current_item;  // Já existe um símbolo com este nome
            }
            current_item = current_item->next;
        }
        return NULL;  // Nome disponível para função
    }
    
    // Outros tipos: busca no escopo local/global ou conflito com função
    while (current_item != NULL) {
        if (strcmp(id, current_item->id_name) == 0) {
            // Encontra no mesmo escopo ou no escopo global
            if (strcmp(scope, current_item->scope) == 0 || 
                strcmp(current_item->scope, "global") == 0) {
                return current_item;  // Conflito de nome no escopo
            }
            // Conflito com função (funções são sempre globais)
            if (current_item->id_type == DECL_FUNC) {
                return current_item;  // Não pode declarar variável com nome de função
            }
        }
        current_item = current_item->next;
    }

    return NULL;  // Nome disponível para declaração
}


void delete_symbol_table(symbol_item_ptr hash_table[]) {
    if (hash_table == NULL) {
        return;
    }

    // Percorre todos os buckets da tabela hash
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        symbol_item_ptr current_item = hash_table[i];
        
        // Libera todos os itens na lista encadeada deste bucket
        while (current_item != NULL) {
            symbol_item_ptr next_item = current_item->next;
            
            // Libera a lista de linhas deste item
            line_node_ptr current_line = current_item->lines;
            while (current_line != NULL) {
                line_node_ptr next_line = current_line->next;
                free(current_line);
                current_line = next_line;
            }
            
            // Libera o próprio item
            free(current_item);
            current_item = next_item;
        }
        
        hash_table[i] = NULL;
    }
    
    // Libera o array da tabela
    free(hash_table);
}


void add_line(symbol_item_ptr item, int line_value) {
    if (item == NULL) {
        return;
    }
    
    // Aloca nova linha
    line_node_t* new_line = (line_node_t*)malloc(sizeof(line_node_t));
    if (new_line == NULL) {
        fprintf(stderr, "ERRO: Falha ao alocar memória para linha %d\n", line_value);
        return;
    }
    
    new_line->line_num = line_value;
    new_line->next = NULL;
    new_line->prev = NULL;

    // Caso 1: Primeira linha do símbolo
    if (item->lines == NULL) {
        item->lines = new_line;
        return;
    }

    // Caso 2: Adiciona no final da lista
    line_node_ptr current_line = item->lines;
    while (current_line->next != NULL) {
        current_line = current_line->next;
    }
    current_line->next = new_line;
    new_line->prev = current_line;
}

unsigned long hash(const char *str) {
    if (str == NULL) {
        return 0;
    }
    
    unsigned long hashValue = 0;
    unsigned long alpha = 1;  // Coeficiente multiplicativo
    int len = strlen(str);

    // Processa string da direita para esquerda
    for (int i = len - 1; i >= 0; i--) {
        hashValue += alpha * (unsigned char)str[i];
        alpha = alpha << HASH_ALPHA_SHIFT;  // Multiplica por 16 (2^4)
    }

    return hashValue % HASH_TABLE_SIZE;
}

static const char* get_data_type_name(data_type_t type) {
    return (type == TYPE_INT) ? "INT" : "VOID";
}

static const char* get_decl_type_name(decl_type_t type) {
    switch (type) {
        case DECL_VAR:         return "VAR";
        case DECL_FUNC:        return "FUN";
        case DECL_ARRAY:       return "VET";
        case DECL_PARAM_VAR:   return "PARAM_VAR";
        case DECL_PARAM_ARRAY: return "PARAM_VET";
        default:               return "DESCONHECIDO";
    }
}

void print_symbol_table(symbol_item_ptr hash_table[]) {
    if (hash_table == NULL || output_file == NULL) {
        return;
    }

    fprintf(output_file, "\n");
    fprintf(output_file, "===============================================================================\n");
    fprintf(output_file, "                         TABELA DE SÍMBOLOS                                  \n");
    fprintf(output_file, "===============================================================================\n\n");

    int total_symbols = 0;
    
    // Percorre todos os buckets da tabela
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        symbol_item_ptr current_item = hash_table[i];
        
        // Processa todos os itens neste bucket
        while (current_item != NULL) {
            total_symbols++;
            
            // Imprime informações do símbolo
            fprintf(output_file, "Símbolo #%d:\n", total_symbols);
            fprintf(output_file, "  Nome: %s\n", current_item->id_name);
            
            // Escopo (exceto para funções que são sempre globais)
            if (current_item->id_type != DECL_FUNC) {
                fprintf(output_file, "  Escopo: %s\n", current_item->scope);
            }
            
            fprintf(output_file, "  Tipo de Dado: %s\n", 
                    get_data_type_name(current_item->data_type));
            fprintf(output_file, "  Tipo de Identificador: %s\n", 
                    get_decl_type_name(current_item->id_type));
            
            // Lista de linhas onde o símbolo aparece
            fprintf(output_file, "  Linhas: ");
            line_node_ptr current_line = current_item->lines;
            while (current_line != NULL) {
                fprintf(output_file, "%d", current_line->line_num);
                if (current_line->next != NULL) {
                    fprintf(output_file, ", ");
                }
                current_line = current_line->next;
            }
            fprintf(output_file, "\n\n");
            
            current_item = current_item->next;
        }
    }
    
    fprintf(output_file, "===============================================================================\n");
    fprintf(output_file, "Total de símbolos: %d\n", total_symbols);
    fprintf(output_file, "===============================================================================\n\n");
}


symbol_item_ptr search_symbol_any(symbol_item_ptr hash_table[], char id[], char scope[]) {
    unsigned int index = hash(id);
    symbol_item_ptr current_item = hash_table[index];

    while (current_item != NULL) {
        if (strcmp(id, current_item->id_name) == 0) {
            // Encontra no escopo especificado ou no escopo global
            if (strcmp(scope, current_item->scope) == 0 || 
                strcmp(current_item->scope, "global") == 0) {
                return current_item;
            }
        }
        current_item = current_item->next;
    }

    return NULL;
}