#include "synthesis.h"

function_memory_t* global = NULL;  /* Pointer to global scope frame */

void initialize_memory(memory_t *memoria) {
    /* Create global scope frame */
    function_memory_t* func_global = (function_memory_t *) malloc(sizeof(function_memory_t));
    if (func_global == NULL) {
        fprintf(stderr, "Error: Failed to allocate global memory frame\n");
        exit(EXIT_FAILURE);
    }
    
    func_global->size = 0;
    func_global->name = strdup("global");
    func_global->next = NULL;
    func_global->var_table = NULL;
    
    global = func_global;  /* Store global reference */

    /* Create parameters frame */
    function_memory_t* parametros = (function_memory_t*)malloc(sizeof(function_memory_t));
    if (parametros == NULL) {
        fprintf(stderr, "Error: Failed to allocate parameters frame\n");
        exit(EXIT_FAILURE);
    }

    parametros->size = 0;
    parametros->name = strdup("parametros");
    parametros->next = NULL;
    parametros->var_table = NULL;

    global->next = parametros;

    memoria->size = 2;
    memoria->funcs = func_global;
}


function_memory_t* insert_function(memory_t *memoria, char * nome_funcao) {
    function_memory_t* funcao = (function_memory_t *) malloc(sizeof(function_memory_t));
    if (funcao == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for function frame\n");
        exit(EXIT_FAILURE);
    }
    
    funcao->size = 0;
    funcao->name = nome_funcao;
    funcao->next = NULL;
    funcao->var_table = NULL;

    /* Insert standard control variables */
    insert_variable(funcao, "Vinculo Controle", VAR_CONTROL);
    insert_variable(funcao, "Endereco Retorno", VAR_RETURN);
    insert_variable(funcao, "Valor Retorno", VAR_RETURN);
    insert_variable(funcao, "Registrador Temporario", VAR_INT);
    insert_variable(funcao, "Registrador $fp", VAR_INT);
    insert_variable(funcao, "Registrador $sp", VAR_INT);

    /* If this is the first function */
    if (memoria->size == 0) {
        memoria->funcs = funcao;
        memoria->size++;
        return funcao;
    }

    /* Append to end of function list */
    function_memory_t* current = memoria->funcs;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = funcao;
    memoria->size++;

    return funcao;
}


void insert_variable(function_memory_t* funcao, char * nome_variavel, var_type_t tipo) {
    if (funcao == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
        fprintf(stderr, "NULL pointer passed to insert_variable\n");
        return;
    }
    
    /* Allocate and initialize new variable */
    variable_t* variavel = (variable_t *) malloc(sizeof(variable_t));
    if (variavel == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for variable\n");
        exit(EXIT_FAILURE);
    }
    
    variavel->type = tipo;
    variavel->name = nome_variavel;
    variavel->next = NULL;
    variavel->is_global = (strcmp(funcao->name, "global") == 0) ? 1 : 0;

    /* If this is the first variable in the function */
    if (funcao->var_table == NULL) {
        funcao->var_table = variavel;
        variavel->index = funcao->size;
        funcao->size++;
        return;
    }

    /* Special handling for function arguments - insert at beginning */
    if (tipo == VAR_INT_ARG || tipo == VAR_ARRAY_ARG) {
        variable_t* current = funcao->var_table;

        /* If the first variable is not an argument, insert at head */
        if (current->type != VAR_INT_ARG && current->type != VAR_ARRAY_ARG) {
            variavel->next = current;
            funcao->var_table = variavel;
        }
        else {
            /* Find the last argument and insert after it */
            while (current->next != NULL && 
                   (current->next->type == VAR_INT_ARG || current->next->type == VAR_ARRAY_ARG)) {
                current = current->next;
            }

            variable_t* temp = current->next;
            current->next = variavel;
            variavel->next = temp;
        }

        funcao->size++;
        
        /* Update all indices after insertion */
        current = funcao->var_table;
        for (int i = 0; current != NULL; i++) {
            current->index = i;
            current = current->next;
        }

        return;
    }

    /* For non-argument variables, append to the end */
    variable_t* current = funcao->var_table;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = variavel;
    variavel->index = funcao->size;
    funcao->size++;
}


variable_t* get_variable(function_memory_t* funcao, char * nome_variavel) {
    if (funcao == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
        fprintf(stderr, "NULL pointer passed to get_variable\n");
        return NULL;
    }
    
    /* Search in current function scope */
    function_memory_t* currentFrame = funcao;
    while (currentFrame != NULL) {
        variable_t* currentVar = currentFrame->var_table;
        while (currentVar != NULL) {
            if (strcmp(currentVar->name, nome_variavel) == 0) {
                return currentVar;
            }
            currentVar = currentVar->next;
        }
        currentFrame = currentFrame->next;
    }

    /* Search in global scope if not found in local scope */
    currentFrame = global;
    while (currentFrame != NULL) {
        variable_t* currentVar = currentFrame->var_table;
        while (currentVar != NULL) {
            if (strcmp(currentVar->name, nome_variavel) == 0) {
                return currentVar;
            }
            currentVar = currentVar->next;
        }
        currentFrame = currentFrame->next;
    }

    /* Variable not found */
    fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
    fprintf(stderr, "Variable '%s' not found\n", nome_variavel);
    return NULL;
}


void delete_temp(function_memory_t* funcao) {
    if (!funcao) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);	
        fprintf(stderr, "NULL pointer passed to apagar_temp\n");
        return;
    }

    variable_t* current = funcao->var_table;
    variable_t* previous = current;
    
    /* Handle empty function frame */
    if (funcao->size == 0) {
        printf("No temporary variables to delete\n");
        return;
    }

    /* Handle single variable case */
    if (funcao->size == 1) {
        if (strcmp(current->name, "Param") == 0) {
            free(current);
            funcao->size--;
            funcao->var_table = NULL;
            return;
        }
    }

    /* Search through variable list for "Param" variables */
    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }

    /* Check if the last variable is a Param */
    if (strcmp(current->name, "Param") == 0) {
        free(current);
        previous->next = NULL;
        funcao->size--;
        return;
    }

    fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
    fprintf(stderr, "Param variables not deleted\n");
}


void imprime_tipo(variable_t* var) {
    switch (var->type) {
        case VAR_INT:
            printf("inteiro");
            break;
        case VAR_INT_ARG:
            printf("inteiroArg");
            break;
        case VAR_ARRAY:
            printf("vetor");
            break;
        case VAR_ARRAY_ARG:
            printf("vetorArg");
            break;
        case VAR_CONTROL:
            printf("controle");
            break;
        case VAR_TEMP:
            printf("temp");
            break;
        case VAR_RETURN:
            printf("retorno");
            break;
        default:
            fprintf(stderr, "Error: Unknown variable type\n");
            break;
    }
}


void imprime_memoria() {
    function_memory_t* currentFunc = memory_vector.funcs;

    for (int i = 0; i < memory_vector.size; i++, currentFunc = currentFunc->next) {
        printf("===============================================\n");
        printf("\t\t%s: %d variables\n", currentFunc->name, currentFunc->size);
        printf("===============================================\n");

        int fp = get_fp(currentFunc);
        int sp = get_sp(currentFunc);
        int flag_sp = 0;
        
        variable_t* currentVar = currentFunc->var_table;
        for (int j = 0; j < currentFunc->size; j++, currentVar = currentVar->next) {
            /* Mark stack pointer position */
            if (j == sp) {
                printf("$sp -> ");
                flag_sp = 1;
            }
            
            /* Mark frame pointer position */
            if (j == fp) {
                printf("$fp -> ");
            }
            
            printf("\t%d: %-25s [$fp + %2d] [$sp - %2d] : ",
                currentVar->index, 
                currentVar->name, 
                get_fp_relation(currentFunc, currentVar), 
                get_sp_relation(currentFunc, currentVar));
            
            imprime_tipo(currentVar); 
            printf(currentVar->is_global ? " global" : " local");
            printf("\n");
        }
        
        /* Show stack pointer if not already marked and not global */
        if (!flag_sp && strcmp(currentFunc->name, "global") != 0) {
            printf("$sp -> \t%d:\n", sp);
        }
    }
    printf("\n");
}


function_memory_t* search_function(memory_t* memoria, char* nome_funcao) {
    if (memoria == NULL || nome_funcao == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
        fprintf(stderr, "NULL pointer passed to search_function\n");
        return NULL;
    }
    
    function_memory_t* current = memoria->funcs;
    while (current != NULL) {
        if (strcmp(current->name, nome_funcao) == 0) {
            return current;
        }
        current = current->next;
    }

    fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
    fprintf(stderr, "Function '%s' not found\n", nome_funcao);
    return NULL;
}


int get_sp(function_memory_t* funcao) {
    if (funcao == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
        fprintf(stderr, "NULL pointer passed to get_sp\n");
        return -1;
    }
    
    /* Stack pointer points to the last element */
    return funcao->size == 0 ? 0 : funcao->size - 1; 
}


int get_fp(function_memory_t* funcao) {
    if (funcao == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
        fprintf(stderr, "NULL pointer passed to get_fp\n");
        return -1;
    }
    
    if (funcao == global) {
        return 0;
    }
    
    /* Frame pointer at position 0 (control linkage) */
    return 0;
}


int get_sp_relation(function_memory_t* funcao, variable_t* var) {
    if (funcao == NULL || var == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
        fprintf(stderr, "NULL pointer passed to get_sp_relation\n");
        return -1;
    }
    
    return get_sp(funcao) - var->index;
}


int get_fp_relation(function_memory_t* funcao, variable_t* var) {
    if (funcao == NULL || var == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET);
        fprintf(stderr, "NULL pointer passed to get_fp_relation\n");
        return -1;
    }

    /* For global scope, return absolute index */
    if (funcao == global) {
        return var->index;
    } 
    
    return var->index - get_fp(funcao);
}


void free_memory_table() {
    function_memory_t* currentFunc = memory_vector.funcs;

    while (currentFunc != NULL) {
        /* Free all variables in this function */
        variable_t* currentVar = currentFunc->var_table;
        while (currentVar != NULL) {
            variable_t* nextVar = currentVar->next;
            free(currentVar);
            currentVar = nextVar;
        }
        
        /* Free the function frame */
        function_memory_t* nextFunc = currentFunc->next;
        free(currentFunc);
        currentFunc = nextFunc;
    }
}
