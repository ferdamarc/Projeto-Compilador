#include "synthesis.h"

// Flag de debug (descomente para ativar logs detalhados)
// #define DEBUG_COMPILER


instruction_t** intermediate_code = NULL;
int num_reg = 1;                      /* Current register number */
int array_index = 0;                  /* Index in instruction array */
int num_label = 0;                    /* Current label number */
int vector_idx = 0;

instruction_t* func_label = NULL;         /* Label for function end */
char func_name[MAXLEXEMA] = "global"; /* Current function name */

address_t* create_address(address_type_t type, int val, char* name, int is_reg) {
    address_t* address = (address_t*) malloc(sizeof(address_t));
    
    if (address == NULL) {
        fprintf(stderr, "Error: Failed to allocate address\n");
        exit(EXIT_FAILURE);
    }
    
    if (type == ADDR_INT_CONST) {
        address->type = ADDR_INT_CONST;
        address->val = val;
        address->is_register = is_reg;
        address->name = NULL;
    }
    else if (type == ADDR_STRING) {
        address->type = ADDR_STRING;
        address->val = 0;
        address->name = strdup(name);
        if (address->name == NULL) {
            fprintf(stderr, "Error: Failed to duplicate string\n");
            free(address);
            exit(EXIT_FAILURE);
        }
    }
    else {
        address->type = ADDR_EMPTY;
        address->val = 0;
        address->name = NULL;
    }

    return address;
}


instruction_t* create_instruction(char* op) {
    instruction_t* instruction = (instruction_t*) malloc(sizeof(instruction_t));
    
    if (instruction == NULL) {
        fprintf(stderr, "Error: Failed to allocate instruction\n");
        exit(EXIT_FAILURE);
    }
    
    instruction->op = op;
    instruction->arg1 = NULL;
    instruction->arg2 = NULL;
    instruction->arg3 = NULL;
    
    return instruction;
}


void add_instruction(instruction_t* instruction) {
    if (array_index >= MAX_INSTRUCTION) {
        fprintf(stderr, "[ERRO] Limite de instrucoes intermediarias excedido (%d)\n", MAX_INSTRUCTION);
        exit(EXIT_FAILURE);
    }
    
    if (instruction == NULL) {
        fprintf(stderr, "[ERRO] Tentativa de adicionar instrucao NULL no indice %d\n", array_index);
        exit(EXIT_FAILURE);
    }
    
    intermediate_code[array_index++] = instruction;
}


instruction_t* create_instruction_label(int labelNum) {
    instruction_t* instrucao = create_instruction("LABEL");
    instrucao->arg1 = create_address(ADDR_INT_CONST, labelNum, NULL, 2);
    instrucao->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}


instruction_t* create_instruction_goto(int labelNum) {
    instruction_t* instrucao = create_instruction("GOTO");
    instrucao->arg1 = create_address(ADDR_INT_CONST, labelNum, NULL, 2);
    instrucao->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}


instruction_t* create_instruction_arithmetic(char* opName, int reg1, int reg2, int destReg) {
    instruction_t* instrucao = NULL;
    
    if (strcmp(opName, "+") == 0) {
        instrucao = create_instruction("ADD");
    } else if (strcmp(opName, "-") == 0) {
        instrucao = create_instruction("SUB");
    } else if (strcmp(opName, "*") == 0) {
        instrucao = create_instruction("MULT");
    } else if (strcmp(opName, "/") == 0) {
        instrucao = create_instruction("DIV");
    } else {
        fprintf(stderr, "ERRO: Operação aritmética desconhecida: %s\n", opName);
        return NULL;
    }
    
    instrucao->arg1 = create_address(ADDR_INT_CONST, reg1, NULL, 1);
    instrucao->arg2 = create_address(ADDR_INT_CONST, reg2, NULL, 1);
    instrucao->arg3 = create_address(ADDR_INT_CONST, destReg, NULL, 1);
    
    return instrucao;
}


instruction_t* create_instruction_relational(char* opName, int reg1, int reg2, int destReg) {
    instruction_t* instrucao = NULL;
    
    if (strcmp(opName, "==") == 0) {
        instrucao = create_instruction("EQ");
    } else if (strcmp(opName, "!=") == 0) {
        instrucao = create_instruction("NEQ");
    } else if (strcmp(opName, ">") == 0) {
        instrucao = create_instruction("GT");
    } else if (strcmp(opName, "<") == 0) {
        instrucao = create_instruction("LT");
    } else if (strcmp(opName, ">=") == 0) {
        instrucao = create_instruction("GET");
    } else if (strcmp(opName, "<=") == 0) {
        instrucao = create_instruction("LET");
    } else {
        fprintf(stderr, "ERRO: Operação relacional desconhecida: %s\n", opName);
        return NULL;
    }
    
    instrucao->arg1 = create_address(ADDR_INT_CONST, reg1, NULL, 1);
    instrucao->arg2 = create_address(ADDR_INT_CONST, reg2, NULL, 1);
    instrucao->arg3 = create_address(ADDR_INT_CONST, destReg, NULL, 1);
    
    return instrucao;
}


instruction_t* create_instruction_loadi(int destReg, int value) {
    instruction_t* instrucao = create_instruction("LOADI");
    instrucao->arg1 = create_address(ADDR_INT_CONST, destReg, NULL, 1);
    instrucao->arg2 = create_address(ADDR_INT_CONST, value, NULL, 0);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}

instruction_t* create_instruction_fun(char* funcName, int numParams) {
    instruction_t* instrucao = create_instruction("FUN");
    instrucao->arg1 = create_address(ADDR_STRING, 0, funcName, 0);
    instrucao->arg2 = create_address(ADDR_INT_CONST, numParams, NULL, 0);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}


instruction_t* create_instruction_end() {
    instruction_t* instrucao = create_instruction("END");
    if (instrucao == NULL) {
        fprintf(stderr, "[ERRO] Falha ao criar instrucao END\n");
        return NULL;
    }
    instrucao->arg1 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucao->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}


instruction_t* create_instruction_ret(int hasReturnValue, int reg) {
    instruction_t* instrucao = create_instruction("RET");
    if (instrucao == NULL) {
        fprintf(stderr, "[ERRO] Falha ao criar instrucao RET\n");
        return NULL;
    }
    
    if (hasReturnValue) {
        instrucao->arg1 = create_address(ADDR_INT_CONST, reg, NULL, 1);
    } else {
        instrucao->arg1 = create_address(ADDR_EMPTY, 0, NULL, 0);
    }
    
    instrucao->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}


instruction_t* create_instruction_alloc(char* varName, char* scope, int size) {
    instruction_t* instrucao = create_instruction("ALLOC");
    instrucao->arg1 = create_address(ADDR_STRING, 0, varName, 0);
    instrucao->arg2 = create_address(ADDR_STRING, 0, scope, 0);
    
    if (size > 0) {
        instrucao->arg3 = create_address(ADDR_INT_CONST, size, NULL, 0);
    } else {
        instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    }
    
    return instrucao;
}


instruction_t* create_instruction_load(int destReg, char* varName, char* scope, int hasIndex, int indexReg) {
    instruction_t* instrucao = create_instruction("LOAD");
    instrucao->arg1 = create_address(ADDR_INT_CONST, destReg, NULL, 1);
    instrucao->arg2 = create_address(ADDR_STRING, 0, varName, 0);
    
    if (hasIndex) {
        instrucao->arg3 = create_address(ADDR_INT_CONST, indexReg, NULL, 1);
    } else {
        instrucao->arg3 = create_address(ADDR_STRING, 0, scope, 0);
    }
    
    return instrucao;
}


instruction_t* create_instruction_store(int srcReg, char* varName, char* scope, int hasIndex, int indexReg) {
    instruction_t* instrucao = create_instruction("STORE");
    instrucao->arg1 = create_address(ADDR_INT_CONST, srcReg, NULL, 1);
    instrucao->arg2 = create_address(ADDR_STRING, 0, varName, 0);
    
    if (hasIndex) {
        instrucao->arg3 = create_address(ADDR_INT_CONST, indexReg, NULL, 1);
    } else {
        instrucao->arg3 = create_address(ADDR_STRING, 0, scope, 0);
    }
    
    return instrucao;
}


instruction_t* create_instruction_arg(int reg, int paramNum) {
    instruction_t* instrucao = create_instruction("ARG");
    instrucao->arg1 = create_address(ADDR_INT_CONST, reg, NULL, 1);
    instrucao->arg2 = create_address(ADDR_INT_CONST, paramNum, NULL, 0);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}


instruction_t* create_instruction_iff(int condReg, int labelNum) {
    instruction_t* instrucao = create_instruction("IFF");
    instrucao->arg1 = create_address(ADDR_INT_CONST, condReg, NULL, 1);
    instrucao->arg2 = create_address(ADDR_INT_CONST, labelNum, NULL, 2);
    instrucao->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    return instrucao;
}

//Desaloca o vetor de codigo intermediario
void deallocate_vector(){
    for(int i = 0; i < MAX_INSTRUCTION && (intermediate_code[i] != NULL); i++){
        if(intermediate_code[i]->arg1 != NULL)
            free(intermediate_code[i]->arg1);
        if(intermediate_code[i]->arg2 != NULL)
            free(intermediate_code[i]->arg2);
        if(intermediate_code[i]->arg3 != NULL)
            free(intermediate_code[i]->arg3);
        free(intermediate_code[i]);
    }
    free(intermediate_code);
}

//Cria o vetor de codigo intermediario
void initialize_vector(){
    intermediate_code = (instruction_t**) malloc(sizeof(instruction_t*) * MAX_INSTRUCTION); // TODO Change to dinamic allocation
    
    for(int i = 0; i < MAX_INSTRUCTION; i++){
        intermediate_code[i] = NULL;
    }

    initialize_registers();
}

//Imprime o vetor de codigo intermediario
void print_intermediate_code(){
    fprintf(output_intermediate_file, "============== Codigo Intermediario ===============\n");
    for(int i = 0; i < MAX_INSTRUCTION && intermediate_code[i] != NULL; i++){
        fprintf(output_intermediate_file, "%s, ", intermediate_code[i]->op);
        if(intermediate_code[i]->arg1 != NULL){
            if(intermediate_code[i]->arg1->type == ADDR_INT_CONST){
                if(intermediate_code[i]->arg1->is_register == 1){
                    fprintf(output_intermediate_file, "$t%d, ", intermediate_code[i]->arg1->val);
                }
                else if(intermediate_code[i]->arg1->is_register == 2){
                    fprintf(output_intermediate_file, "L%d, ", intermediate_code[i]->arg1->val);
                }
                else{
                    fprintf(output_intermediate_file, "%d, ", intermediate_code[i]->arg1->val);
                }
            }
            else if(intermediate_code[i]->arg1->type == ADDR_STRING){
                fprintf(output_intermediate_file, "%s, ", intermediate_code[i]->arg1->name);
            }
            else{
                fprintf(output_intermediate_file, "-, ");
            }
        }
        else{
            fprintf(output_intermediate_file, "-, ");
        }
        if(intermediate_code[i]->arg2 != NULL){
            if(intermediate_code[i]->arg2->type == ADDR_INT_CONST){
                if(intermediate_code[i]->arg2->is_register == 1){
                    fprintf(output_intermediate_file, "$t%d, ", intermediate_code[i]->arg2->val);
                }
                else if(intermediate_code[i]->arg2->is_register == 2){
                    fprintf(output_intermediate_file, "L%d, ", intermediate_code[i]->arg2->val);
                }
                else{
                    fprintf(output_intermediate_file, "%d, ", intermediate_code[i]->arg2->val);
                }
            }
            else if(intermediate_code[i]->arg2->type == ADDR_STRING){
                fprintf(output_intermediate_file, "%s, ", intermediate_code[i]->arg2->name);
            }
            else{
                fprintf(output_intermediate_file, "-, ");
            }
        }
        else{
            fprintf(output_intermediate_file, "-, ");
        }
        if(intermediate_code[i]->arg3 != NULL){
            if(intermediate_code[i]->arg3->type == ADDR_INT_CONST){
                if(intermediate_code[i]->arg3->is_register == 1)
                    fprintf(output_intermediate_file, "$t%d\n", intermediate_code[i]->arg3->val);
                else
                    fprintf(output_intermediate_file, "%d\n", intermediate_code[i]->arg3->val);
            }
            else if(intermediate_code[i]->arg3->type == ADDR_STRING){
                fprintf(output_intermediate_file, "%s\n", intermediate_code[i]->arg3->name);
            }
            else{
                fprintf(output_intermediate_file, "-\n");
            }
        }
        else{
            fprintf(output_intermediate_file, "-\n");
        }
    }
}


void intermediate_code_decl_if(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]) {
    instruction_t* instrucaoIF = NULL;
    instruction_t* instrucaoGoto = NULL;
    instruction_t* instrucaoLabel1 = NULL;
    instruction_t* instrucaoLabel2 = NULL;

    /* Generate code for condition */
    create_intermediate_code(arvoreSintatica->children[0], tabelaHash, 1);

    /* Create IFF (IF False) instruction using helper */
    instrucaoIF = create_instruction_iff(num_reg, num_label);
    add_instruction(instrucaoIF);

    /* Pre-create labels for control flow */
    instrucaoLabel1 = create_instruction_label(num_label);
    num_label++;

    instrucaoGoto = create_instruction_goto(num_label);
    instrucaoLabel2 = create_instruction_label(num_label);
    num_label++;

    /* Generate code for IF body (true branch) */
    create_intermediate_code(arvoreSintatica->children[1], tabelaHash, 1);

    /* Handle ELSE clause if present */
    if (arvoreSintatica->children[2] != NULL) {
        add_instruction(instrucaoGoto);
        add_instruction(instrucaoLabel1);

        /* Generate code for ELSE body (false branch) */
        create_intermediate_code(arvoreSintatica->children[2], tabelaHash, 1);

        add_instruction(instrucaoLabel2);
    }
    else {
        /* No ELSE clause, just add the false label */
        add_instruction(instrucaoLabel1);
    }
}

void intermediate_code_decl_func(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    ast_node_ptr noParam = arvoreSintatica->children[0];
    instruction_t* func = NULL;
    instruction_t* param = NULL;

    int numParam = 0;

    strcpy(func_name, arvoreSintatica->children[1]->lexeme);

#ifdef DEBUG_COMPILER
    printf("[LOG CI] Gerando funcao: %s\n", func_name);
#endif

    /* Create FUN instruction with placeholder for parameter count */
    func = create_instruction("FUN");
    func->arg1 = create_address(ADDR_STRING, 0, arvoreSintatica->lexeme, 0);
    func->arg2 = create_address(ADDR_STRING, 0, arvoreSintatica->children[1]->lexeme, 0);
    intermediate_code[array_index] = func;
    array_index++;
    
    /* Create label for function end */
    func_label = create_instruction_label(num_label);
    num_label++;

    /* Handle functions with no parameters (void) */
    if(arvoreSintatica->children[0]->decl_type == DECL_PARAM_VOID){
        create_intermediate_code(arvoreSintatica->children[1]->children[0], tabelaHash, 1);

        add_instruction(func_label);

        /* Add END instruction */
        instruction_t* endInstr = create_instruction_end();
        endInstr->arg1 = create_address(ADDR_STRING, 0, arvoreSintatica->children[1]->lexeme, 0);
        add_instruction(endInstr);

        func->arg3 = create_address(ADDR_INT_CONST, numParam, NULL, 0);
        return;
    }

    /* Count parameters and create ARG instructions */
    while(noParam != NULL){
        numParam++;
        param = create_instruction("ARG");

        if(noParam->decl_type == DECL_PARAM_VAR)
            param->arg1 = create_address(ADDR_STRING, 0, "INT", 0);
        else 
            param->arg1 = create_address(ADDR_STRING, 0, "VET", 0);

        param->arg2 = create_address(ADDR_STRING, 0, noParam->children[0]->lexeme, 0);
        param->arg3 = create_address(ADDR_STRING, 0, arvoreSintatica->children[1]->lexeme, 0);

        intermediate_code[array_index] = param;
        array_index++;
        
        noParam = noParam->sibling;
    }

    func->arg3 = create_address(ADDR_INT_CONST, numParam, NULL, numParam);

    /* Load parameters into registers */
    noParam = arvoreSintatica->children[0];
    while(noParam != NULL){
        num_reg = check_registers(noParam->children[0]->lexeme, arvoreSintatica->children[1]->lexeme, 1);
    
        param = create_instruction("LOAD");
        param->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
        param->arg2 = create_address(ADDR_STRING, 0, noParam->children[0]->lexeme, 0);
        param->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
        intermediate_code[array_index] = param;
        array_index++;

        noParam = noParam->sibling;
    }

    /* Generate code for function body */
    create_intermediate_code(arvoreSintatica->children[1]->children[0], tabelaHash, 1);

    add_instruction(func_label);

    intermediate_code[array_index] = create_instruction("END");
    intermediate_code[array_index]->arg1 = create_address(ADDR_STRING, 0, arvoreSintatica->children[1]->lexeme, 0);
    array_index++;
}

void intermediate_code_decl_var(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* var = NULL;
    char* varName = arvoreSintatica->children[0]->lexeme;
    char* scope = NULL;
    int arraySize = 0;


#ifdef DEBUG_COMPILER
    printf("[LOG CI] Alocando variavel: %s\n", varName);
#endif

    /* Determine variable scope */
    symbol_item_ptr itemFunc = search_symbol_id(tabelaHash, varName);

    if(itemFunc == NULL){
        scope = "escopo";
    }
    else if(strcmp(itemFunc->scope, "global") == 0){
        scope = "global";
    }
    else{
        scope = func_name;
    }
    
    /* Get array size if it's an array declaration */
    if(arvoreSintatica->decl_type == DECL_ARRAY){
        arraySize = atoi(arvoreSintatica->children[1]->lexeme);
    }

    /* Create and add ALLOC instruction */
    var = create_instruction_alloc(varName, scope, arraySize);
    add_instruction(var);
}

void intermediate_code_decl_return(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* ret = NULL;
    instruction_t* gotoInstr = NULL;
    int hasReturnValue = 0;
    int returnReg = 0;


#ifdef DEBUG_COMPILER
    printf("[LOG CI] Gerando retorno\n");
#endif

    /* Check if function returns a value */
    if(arvoreSintatica->decl_type == DECL_RETURN_INT){
        create_intermediate_code(arvoreSintatica->children[0], tabelaHash, 1);
        hasReturnValue = 1;
        returnReg = num_reg;
    }

    /* Create and add RET instruction */
    ret = create_instruction_ret(hasReturnValue, returnReg);
    add_instruction(ret);

    /* Create and add GOTO to function end label */
    gotoInstr = create_instruction_goto(func_label->arg1->val);
    add_instruction(gotoInstr);
}

void intermediate_code_expr_op(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* op = NULL;
    char NomeOp[MAXLEXEMA];
    int reg1, reg2;

    strcpy(NomeOp, arvoreSintatica->lexeme);

#ifdef DEBUG_COMPILER
    printf("[LOG CI] Gerando operacao aritmetica: %s\n", NomeOp);
#endif

    /* Generate code for left operand */
    create_intermediate_code(arvoreSintatica->children[0], tabelaHash, 1);
    reg1 = num_reg;

    /* Generate code for right operand */
    create_intermediate_code(arvoreSintatica->children[1], tabelaHash, 1);
    reg2 = num_reg;

    /* Allocate register for result */
    num_reg = check_registers(NULL, NULL, 1);

    /* Create and add arithmetic instruction */
    op = create_instruction_arithmetic(NomeOp, reg1, reg2, num_reg);
    if (op != NULL) {
        add_instruction(op);
    }
}

void intermediate_code_expr_const(ast_node_ptr arvoreSintativa, symbol_item_ptr tabelaHash[]){
    instruction_t* constante = NULL;

    /* Optimization: constant 0 uses $zero register */
    if(strcmp(arvoreSintativa->lexeme, "0") == 0){
        num_reg = $zero;
        return;
    }

    /* Allocate register for constant */
    num_reg = check_registers(NULL, NULL, 1);
        
    /* Create and add LOADI instruction */
    constante = create_instruction_loadi(num_reg, atoi(arvoreSintativa->lexeme));
    add_instruction(constante);
}

void intermediate_code_expr_relop(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* instrucaoOp = NULL;
    int reg1, reg2;
    
    /* Generate code for left operand */
    create_intermediate_code(arvoreSintatica->children[0], tabelaHash, 1);
    reg1 = num_reg;
    
    /* Generate code for right operand */
    create_intermediate_code(arvoreSintatica->children[1], tabelaHash, 1);
    reg2 = num_reg;

    /* Allocate register for result */
    num_reg = check_registers(NULL, NULL, 1);

    /* Create and add relational instruction */
    instrucaoOp = create_instruction_relational(arvoreSintatica->lexeme, reg1, reg2, num_reg);
    if (instrucaoOp != NULL) {
        add_instruction(instrucaoOp);
    }
}

void intermediate_code_expr_id(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* instrucaoId = NULL;
    
    if(arvoreSintatica->expr_type == EXPR_ARRAY){
        instrucaoId = create_instruction("LOAD");

        create_intermediate_code(arvoreSintatica->children[0], tabelaHash, 1);
        instrucaoId->arg3 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
        vector_idx = num_reg;

        num_reg = check_registers(NULL, NULL, 1);
        
        instrucaoId->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1); // Valor do registrador alterado
        instrucaoId->arg2 = create_address(ADDR_STRING, 0, arvoreSintatica->lexeme, 0);

    }
    else if(arvoreSintatica->expr_type == EXPR_ID){  
        //Otimizacao: Adicionando variavel no vetor de variaveis de registradores
        /* Primeiro busca se a variavel ja esta no vetor de registradores, se nao estiver, deve ser adicionada
        Caso de algum erro ao adicionar, mostrar um erro */
        symbol_item_ptr varEscopo = NULL;
        if(!(varEscopo = search_symbol_id(tabelaHash, arvoreSintatica->lexeme))){
            printf(ANSI_COLOR_RED "ERRO: " ANSI_COLOR_RESET);
            printf("Escopo da variavel '%s' nao encontrada", arvoreSintatica->lexeme);
            num_reg = -1;
        }
        if(!strcmp(varEscopo->scope, "global")){
            num_reg = check_registers(arvoreSintatica->lexeme, "global", 1);
        }
        else{
            num_reg = check_registers(arvoreSintatica->lexeme, func_name, 1);
        }
        
        instrucaoId = create_instruction("LOAD");
        instrucaoId->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
        instrucaoId->arg2 = create_address(ADDR_STRING, 0, arvoreSintatica->lexeme, 0);
        instrucaoId->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    }

    if(instrucaoId != NULL){
        intermediate_code[array_index] = instrucaoId;
        array_index++;
    }
}

void intermediate_code_expr_call(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* instrucaoCall = NULL;
    instruction_t* instrucaoParam = NULL;
    ast_node_ptr noAux = arvoreSintatica->children[0];   

    int numParam = 0;

    if (strcmp(arvoreSintatica->lexeme, "show_lcd") == 0) {
        ast_node_ptr argumento = arvoreSintatica->children[0];
        if (argumento != NULL && argumento->sibling == NULL) {
            if (argumento->expr_type == EXPR_CONST) {
                instruction_t* instrucaoDisp = create_instruction("DISP_CONST");
                instrucaoDisp->arg1 = create_address(ADDR_INT_CONST, atoi(argumento->lexeme), NULL, 0);
                instrucaoDisp->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
                instrucaoDisp->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
                intermediate_code[array_index++] = instrucaoDisp;
                return;
            }
            else {
                // Modo Variável: Gera o código para carregar a variável e depois DISP_VAR
                // Primeiro, gera o código intermediário para o argumento (que colocará o valor em um registrador)
                create_intermediate_code(argumento, tabelaHash, 0);
                instruction_t* instrucaoDisp = create_instruction("DISP_VAR");
                // O registrador que contém o valor da variável vai para arg1
                instrucaoDisp->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
                instrucaoDisp->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
                instrucaoDisp->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
                intermediate_code[array_index++] = instrucaoDisp;
                return;
            }
        } else {
             // Tratamento de erro: show_lcd deve ter exatamente um argumento
             printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao show_lcd espera exatamente um argumento.\n");
             return;
        }
    }

    if (strcmp(arvoreSintatica->lexeme, "os_jump_to") == 0) {
        ast_node_ptr argumento = arvoreSintatica->children[0];
        if (argumento == NULL) {
            instruction_t* instrucaoOs = create_instruction("OS_JUMP_TO");
            intermediate_code[array_index++] = instrucaoOs;
            return;
        }
    }

    if (strcmp(arvoreSintatica->lexeme, "no_op") == 0) {
        if (arvoreSintatica->children[0] == NULL) {
            instruction_t* instrucaoNop = create_instruction("NO_OP");
            intermediate_code[array_index++] = instrucaoNop;
            return;
        }
    }

    if (strcmp(arvoreSintatica->lexeme, "os_set_dm_base") == 0) {
        ast_node_ptr argumento = arvoreSintatica->children[0];
        
        if (argumento != NULL && argumento->sibling == NULL) {
            // Gera código para carregar o valor do offset em um registrador
            create_intermediate_code(argumento, tabelaHash, 0);
            
            instruction_t* instrucaoSetBase = create_instruction("OS_SET_DM_BASE");
            instrucaoSetBase->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
            instrucaoSetBase->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
            instrucaoSetBase->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
            intermediate_code[array_index++] = instrucaoSetBase;
            return;
        } else {
            printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao os_set_dm_base espera exatamente um argumento.\n");
            exit(1);
        }
    }

    if (strcmp(arvoreSintatica->lexeme, "os_set_im_base") == 0) {
        ast_node_ptr argumento = arvoreSintatica->children[0];
        
        if (argumento != NULL && argumento->sibling == NULL) {
            // Gera código para carregar o valor do offset em um registrador
            create_intermediate_code(argumento, tabelaHash, 0);
            
            instruction_t* instrucaoSetImBase = create_instruction("OS_SET_IM_BASE");
            instrucaoSetImBase->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
            instrucaoSetImBase->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
            instrucaoSetImBase->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
            intermediate_code[array_index++] = instrucaoSetImBase;
            return;
        } else {
            printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao os_set_im_base espera exatamente um argumento.\n");
            exit(1);
        }
    }

    if (strcmp(arvoreSintatica->lexeme, "os_save_return") == 0) {
        if (arvoreSintatica->children[0] == NULL) {
            instruction_t* instrucaoSaveReturn = create_instruction("OS_SAVE_RETURN");
            intermediate_code[array_index++] = instrucaoSaveReturn;
            return;
        } else {
            printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao os_save_return nao espera argumentos.\n");
            exit(1);
        }
    }

    // Instrução para salvar o contexto atual, salvar todos os registradores
    if (strcmp(arvoreSintatica->lexeme, "os_save_context") == 0) {
        if (arvoreSintatica->children[0] == NULL) {
            instruction_t* instrucaoSaveContext = create_instruction("OS_SAVE_CONTEXT");
            intermediate_code[array_index++] = instrucaoSaveContext;
            return;
        } else {
            printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao os_save_context nao espera argumentos.\n");
            exit(1);
        }
    }

    // Instrução para restaurar o contexto salvo, restaurar todos os registradores
    if (strcmp(arvoreSintatica->lexeme, "os_restore_context") == 0) {
        if (arvoreSintatica->children[0] == NULL) {
            instruction_t* instrucaoRestoreContext = create_instruction("OS_RESTORE_CONTEXT");
            intermediate_code[array_index++] = instrucaoRestoreContext;
            return;
        } else {
            printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao os_restore_context nao espera argumentos.\n");
            exit(1);
        }
    }

    // Instrução "set_interr_timer" para configurar o timer de interrupção
    if (strcmp(arvoreSintatica->lexeme, "set_interr_timer") == 0) {
        ast_node_ptr argumento = arvoreSintatica->children[0];
        if (argumento != NULL && argumento->sibling == NULL) {
            instruction_t* instrucaoSetTimer = create_instruction("SET_INTERR_TIMER");
            instrucaoSetTimer->arg1 = create_address(ADDR_INT_CONST, atoi(argumento->lexeme), NULL, 0);
            instrucaoSetTimer->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
            instrucaoSetTimer->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
            intermediate_code[array_index++] = instrucaoSetTimer;
            return;
        }
        else {
            printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao set_interr_timer espera exatamente um argumento.\n");
            exit(1);
        }
    }

    // Instrução para carregar o valor de um registrador dado no argumento para o $k0
    if (strcmp(arvoreSintatica->lexeme, "os_set_pc") == 0) {
        ast_node_ptr argumento = arvoreSintatica->children[0];
        if (argumento != NULL && argumento->sibling == NULL) {
            // Gera código para carregar o valor do argumento em um registrador
            create_intermediate_code(argumento, tabelaHash, 0);
            
            instruction_t* instrucaoSetPc = create_instruction("OS_SET_PC");
            instrucaoSetPc->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
            instrucaoSetPc->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
            instrucaoSetPc->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
            intermediate_code[array_index++] = instrucaoSetPc;
            return;
        } else {
            printf(ANSI_COLOR_RED "ERRO SEMANTICO: " ANSI_COLOR_RESET "Funcao os_set_pc espera exatamente um argumento.\n");
            exit(1);
        }
    }

    instrucaoCall = create_instruction("CALL");
    instrucaoCall->arg1 = create_address(ADDR_STRING, 0, arvoreSintatica->lexeme, 0);

    while(noAux !=  NULL){
        instrucaoParam = create_instruction("PARAM");
        
        symbol_item_ptr itemAux = search_symbol_any(tabelaHash, noAux->lexeme, func_name);
        if(itemAux != NULL && (itemAux->id_type == DECL_ARRAY || itemAux->id_type == DECL_PARAM_ARRAY) && noAux->children[0] == NULL){
            num_reg = check_registers(NULL, NULL, 1);
            instrucaoParam->arg2 = create_address(ADDR_STRING, 0, "VET", 0);
            instrucaoParam->arg3 = create_address(ADDR_STRING, 0, noAux->lexeme, 0);
        }
        else{
            create_intermediate_code(noAux, tabelaHash, 0);   
            instrucaoParam->arg2 = create_address(ADDR_STRING, 0, "INT", 0);
            instrucaoParam->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
        }
        instrucaoParam->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);       

        intermediate_code[array_index] = instrucaoParam;
        array_index++;
        
        noAux = noAux->sibling;

        numParam++;
    }
    
    instrucaoCall->arg2 = create_address(ADDR_INT_CONST, numParam, NULL, 0);
    
    symbol_item_ptr itemAux = search_symbol_func(tabelaHash, arvoreSintatica->lexeme);
    if(itemAux != NULL && itemAux->data_type == TYPE_VOID){
        instrucaoCall->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    }
    else if(itemAux != NULL && itemAux->data_type == TYPE_INT){
        num_reg = check_registers(NULL, NULL, 1);

        instrucaoCall->arg3 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);
        // num_reg++;
    }

    intermediate_code[array_index] = instrucaoCall;
    array_index++;
}

void intermediate_code_expr_assign(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* instrucaoAtrib = NULL;
    instruction_t* instrucaoStore = NULL;

    instrucaoAtrib = create_instruction("ASSIGN");


    create_intermediate_code(arvoreSintatica->children[0], tabelaHash, 1);
    /*if(arvoreSintatica->children[0]->expr_type == EXPR_ARRAY){
        create_intermediate_code(arvoreSintatica->children[0]->children[0], tabelaHash, 1);
    }
    else{
        
    }*/
    
    instrucaoAtrib->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);

    instrucaoStore = create_instruction("STORE");
    instrucaoStore->arg1 = create_address(ADDR_STRING, 0, arvoreSintatica->children[0]->lexeme, 0);
    instrucaoStore->arg2 = create_address(ADDR_INT_CONST, instrucaoAtrib->arg1->val, NULL, 1);

    if(arvoreSintatica->children[0]->expr_type == EXPR_ARRAY){
        instrucaoStore->arg3 = create_address(ADDR_INT_CONST, vector_idx, NULL, 1);
    }
    else{
        instrucaoStore->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    }

    create_intermediate_code(arvoreSintatica->children[1], tabelaHash, 1);
    
    instrucaoAtrib->arg2 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);

    intermediate_code[array_index] = instrucaoAtrib;
    array_index++;    

    intermediate_code[array_index] = instrucaoStore;
    array_index++;
}

void intermediate_code_decl_while(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[]){
    instruction_t* instrucaoIFF = NULL;
    instruction_t* instrucaoGOTO = NULL;
    instruction_t* instrucaoLabel1 = NULL;
    instruction_t* instrucaoLabel2 = NULL;
    
    instrucaoLabel1 = create_instruction("LABEL");
    instrucaoLabel1->arg1 = create_address(ADDR_INT_CONST, num_label, NULL, 2);
    instrucaoLabel1->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucaoLabel1->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);

    intermediate_code[array_index] = instrucaoLabel1;
    array_index++;

    instrucaoGOTO = create_instruction("GOTO");
    instrucaoGOTO->arg1 = create_address(ADDR_INT_CONST, num_label, NULL, 2);
    instrucaoGOTO->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucaoGOTO->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);

    num_label++;

    instrucaoLabel2 = create_instruction("LABEL");
    instrucaoLabel2->arg1 = create_address(ADDR_INT_CONST, num_label, NULL, 2);
    instrucaoLabel2->arg2 = create_address(ADDR_EMPTY, 0, NULL, 0);
    instrucaoLabel2->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);
    num_label++;

    instrucaoIFF = create_instruction("IFF");

    instrucaoIFF->arg2 = create_address(ADDR_INT_CONST, num_label-1, NULL, 2);
    instrucaoIFF->arg3 = create_address(ADDR_EMPTY, 0, NULL, 0);

    create_intermediate_code(arvoreSintatica->children[0], tabelaHash, 1);

    instrucaoIFF->arg1 = create_address(ADDR_INT_CONST, num_reg, NULL, 1);

    intermediate_code[array_index] = instrucaoIFF;
    array_index++;

    create_intermediate_code(arvoreSintatica->children[1], tabelaHash, 1);

    intermediate_code[array_index] = instrucaoGOTO;
    array_index++;

    intermediate_code[array_index] = instrucaoLabel2;
    array_index++;
}


//Funcao que analisa a arvore sintatica e a tabela de simbolos e gera o codigo intermediario de tres enderecos
void create_intermediate_code(ast_node_ptr arvoreSintatica, symbol_item_ptr tabelaHash[], int boolean){
    if(arvoreSintatica == NULL){
        return;
    }

    if(arvoreSintatica->node_type == NODE_DECLARATION){
        if(arvoreSintatica->decl_type == DECL_FUNC){
            intermediate_code_decl_func(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->decl_type == DECL_VAR || arvoreSintatica->decl_type == DECL_ARRAY){
            intermediate_code_decl_var(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->decl_type == DECL_IF){
            intermediate_code_decl_if(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->decl_type == DECL_WHILE){
            intermediate_code_decl_while(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->decl_type == DECL_RETURN_INT || arvoreSintatica->decl_type == DECL_RETURN_VOID){
            intermediate_code_decl_return(arvoreSintatica, tabelaHash);
        }
    }
    else if (arvoreSintatica->node_type == NODE_EXPRESSION){
        if(arvoreSintatica->expr_type == EXPR_OP){
            intermediate_code_expr_op(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->expr_type == EXPR_CONST){
            intermediate_code_expr_const(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->expr_type == EXPR_OP_REL){
            intermediate_code_expr_relop(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->expr_type == EXPR_ID || arvoreSintatica->expr_type == EXPR_ARRAY){
            intermediate_code_expr_id(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->expr_type == EXPR_CALL){
            intermediate_code_expr_call(arvoreSintatica, tabelaHash);
        }
        else if(arvoreSintatica->expr_type == EXPR_ASSIGN){
            intermediate_code_expr_assign(arvoreSintatica, tabelaHash);
        }
    }
    
    if(boolean == 1){
        create_intermediate_code(arvoreSintatica->sibling, tabelaHash, 1);
    }
}