#include "synthesis.h"

memory_t memory_vector;                /* Global memory management structure */
function_memory_t *current_func = NULL; /* Pointer to current function */

void generate_assembly(instruction_t* instruction);


void add_assembly_instruction(assembly_t* instruction) {
    if (assembly_index >= MAX_ASSEMBLY) {
        fprintf(stderr, "Error: Assembly array overflow\n");
        exit(EXIT_FAILURE);
    }
    assembly_instructions[assembly_index++] = instruction;
}

assembly_t* create_r_instruction(char* name, int rd, int rs, int rt) {
    assembly_t* instruction = create_assembly_node(INSTR_TYPE_R, name);
    instruction->type_r->rd = rd;
    instruction->type_r->rs = rs;
    instruction->type_r->rt = rt;
    return instruction;
}


assembly_t* create_i_instruction(char* name, int rt, int rs, int immediate) {
    assembly_t* instruction = create_assembly_node(INSTR_TYPE_I, name);
    instruction->type_i->rt = rt;
    instruction->type_i->rs = rs;
    instruction->type_i->immediate = immediate;
    instruction->type_i->label = -1;
    return instruction;
}


assembly_t* create_j_instruction(char* name, char* label) {
    assembly_t* instruction = create_assembly_node(INSTR_TYPE_J, name);
    instruction->type_j->label_immediate = strdup(label);
    return instruction;
}


void assembly() {
    initialize_assembly();


    /* Create initial jump to main function */
    assembly_t *jumpMain = create_j_instruction("j", "main");
    add_assembly_instruction(jumpMain);

    /* Process all intermediate code instructions */
    for (int i = 0; i < array_index; i++) {
        generate_assembly(intermediate_code[i]);
    }

}

int opRelacionais(instruction_t* instruction, assembly_t** new_instruction) {
    /* Verify arguments before accessing */
    if (instruction->arg1 == NULL || instruction->arg2 == NULL || instruction->arg3 == NULL) {
        return 0;
    }
    
    int rd = instruction->arg3->val;
    int rs = instruction->arg1->val;
    int rt = instruction->arg2->val;

    if(strcmp(instruction->op, "EQ") == 0){
        /* EQ: xor rd, rs, rt; slti rd, rd, 1 */
        (*new_instruction) = create_r_instruction("xor", rd, rs, rt);
        assembly_instructions[assembly_index++] = *new_instruction;

        (*new_instruction) = create_i_instruction("slti", rd, rd, 1);
    }
    else if(strcmp(instruction->op, "LT") == 0){
        /* LT: slt rd, rs, rt */
        (*new_instruction) = create_r_instruction("slt", rd, rs, rt);
    }
    else if(strcmp(instruction->op, "NEQ") == 0){
        /* NEQ: slt $temp, rs, rt; slt rd, rt, rs; or rd, $temp, rd */
        (*new_instruction) = create_r_instruction("slt", $temp, rs, rt);
        assembly_instructions[assembly_index++] = *new_instruction;

        (*new_instruction) = create_r_instruction("slt", rd, rt, rs);
        assembly_instructions[assembly_index++] = *new_instruction;

        (*new_instruction) = create_r_instruction("or", rd, $temp, rd);
    }
    else if(strcmp(instruction->op, "GT") == 0){
        /* GT: slt rd, rt, rs (swap operands) */
        (*new_instruction) = create_r_instruction("slt", rd, rt, rs);
    }
    else if(strcmp(instruction->op, "GET") == 0){
        /* GET: slt rd, rs, rt; xori rd, rd, 1 (negate result) */
        (*new_instruction) = create_r_instruction("slt", rd, rs, rt);
        assembly_instructions[assembly_index++] = *new_instruction;

        (*new_instruction) = create_i_instruction("xori", rd, rd, 1);
    }
    else if(strcmp(instruction->op, "LET") == 0){
        /* LET: slt rd, rt, rs; xori rd, rd, 1 (swap and negate) */
        (*new_instruction) = create_r_instruction("slt", rd, rt, rs);
        assembly_instructions[assembly_index++] = *new_instruction;

        (*new_instruction) = create_i_instruction("xori", rd, rd, 1);
    }
    else{
        return 0;
    }

    return 1;
}

int opAritmeticos(instruction_t* instruction, assembly_t** new_instruction){
    /* Verify arguments before accessing */
    if (instruction->arg1 == NULL || instruction->arg2 == NULL || instruction->arg3 == NULL) {
        return 0;
    }
    
    int rd = instruction->arg3->val;
    int rs = instruction->arg1->val;
    int rt = instruction->arg2->val;
    
    /* Map intermediate code operation to assembly instruction */
    char* asmOp = NULL;
    
    if(strcmp(instruction->op, "ADD") == 0){
        asmOp = "add";
    }
    else if(strcmp(instruction->op, "SUB") == 0){
        asmOp = "sub";
    }
    else if(strcmp(instruction->op, "MULT") == 0){
        asmOp = "mult";
    }
    else if(strcmp(instruction->op, "DIV") == 0){
        asmOp = "div";
    }
    else if(strcmp(instruction->op, "AND") == 0){
        asmOp = "and";
    }
    else if(strcmp(instruction->op, "OR") == 0){
        asmOp = "or";
    }
    else{
        return 0;
    }
    
    /* Create R-type instruction using helper */
    *new_instruction = create_r_instruction(asmOp, rd, rs, rt);
    return 1;
}


void generate_assembly(instruction_t* instruction){
    assembly_t* new_instruction = NULL;

    if (instruction == NULL) {
        fprintf(stderr, "[ERRO assembly_t] Instrucao NULL recebida\n");
        return;
    }

    if (instruction->op == NULL) {
        fprintf(stderr, "[ERRO assembly_t] Operacao NULL na instruction\n");
        return;
    }

    if(opAritmeticos(instruction, &new_instruction)){
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(opRelacionais(instruction, &new_instruction)){
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "ASSIGN")){        
        if (instruction->arg1 == NULL || instruction->arg2 == NULL) {
            fprintf(stderr, "[ERRO assembly_t] ASSIGN com argumentos NULL\n");
            return;
        }

        new_instruction = create_r_instruction("add", instruction->arg1->val, $zero, instruction->arg2->val);
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "LOADI")){
        int aux_register = $zero;
        
        if (instruction->arg2->val > 0xFFFF) {
            // Se o valor for maior que 16bits, precisamos adicionar o valor
            // em dois passos: primeiro o valor alto, depois o valor baixo.
            // Usar um ori para carregar primeiramente o valor alto, shiftar os bits
            // e depois adicionar o valor baixo
            
            aux_register = instruction->arg1->val; // Guardar o registrador de destino
            
            new_instruction = create_assembly_node(INSTR_TYPE_I, "ori");
            new_instruction->type_i->rt = instruction->arg1->val; 
            new_instruction->type_i->rs = $zero; 
            new_instruction->type_i->immediate = (instruction->arg2->val >> 16) & 0xFFFF; // Pega os 16 bits mais altos
            assembly_instructions[assembly_index++] = new_instruction;

            new_instruction = create_assembly_node(INSTR_TYPE_R, "sll");
            new_instruction->type_r->rd = instruction->arg1->val; 
            new_instruction->type_r->rs = instruction->arg1->val; 
            new_instruction->type_r->rt = $zero; // Qualquer registrador
            new_instruction->type_r->shamt = 16; // Shiftar 16 bits
            assembly_instructions[assembly_index++] = new_instruction;

        }

        new_instruction = create_assembly_node(INSTR_TYPE_I, "ori");
        new_instruction->type_i->rt = instruction->arg1->val;
        new_instruction->type_i->rs = aux_register;
        new_instruction->type_i->immediate = instruction->arg2->val;
        assembly_instructions[assembly_index++] = new_instruction;

    }
    else if(!strcmp(instruction->op, "ALLOC")){
        function_memory_t* funcao = search_function(&memory_vector, instruction->arg2->name);
        int count = 0;

        if(!instruction->arg3){
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("NULL no argumento 3\n");
            return;
        }
        
        if(instruction->arg3->type == ADDR_EMPTY){
            insert_variable(funcao, instruction->arg1->name, VAR_INT);
            count = 1;
        }
        else{
            for(int i = 0; i < instruction->arg3->val; i++){
                insert_variable(funcao, instruction->arg1->name, VAR_ARRAY);	
            }
            count = instruction->arg3->val;
        }

        if (strcmp(current_func->name, "main") == 0) {
            int immediate = instruction->arg3->type == ADDR_EMPTY ? 1 : instruction->arg3->val;
            new_instruction = create_i_instruction("addi", $sp, $sp, immediate);
            assembly_instructions[assembly_index++] = new_instruction;
        }

    }
    else if(!strcmp(instruction->op, "ARG")){
        function_memory_t* funcao = search_function(&memory_vector, instruction->arg3->name);
        
        if(!strcmp(instruction->arg1->name, "INT")) 
            insert_variable(funcao, instruction->arg2->name, VAR_INT_ARG);
        else insert_variable(funcao, instruction->arg2->name, VAR_ARRAY_ARG);

        if (strcmp(current_func->name, "main") == 0) {
            new_instruction = create_i_instruction("addi", $sp, $sp, 1);
            assembly_instructions[assembly_index++] = new_instruction;
        }
        
    }	
    else if(!strcmp(instruction->op, "IFF")){
        new_instruction = create_i_instruction("beq", instruction->arg1->val, $zero, 0);
        new_instruction->type_i->label = instruction->arg2->val;
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "LABEL")){
        char* auxLabel = (char*) malloc(sizeof(char) * 12);
        sprintf(auxLabel, "Label %d", instruction->arg1->val);
        new_instruction = create_assembly_node(INSTR_TYPE_LABEL, auxLabel); 
        new_instruction->type_label->address = instruction->arg1->val;
        new_instruction->type_label->is_dynamic = 1;

        char label[26];
        sprintf(label, "Label %d", instruction->arg1->val);
        add_label(label, assembly_index);

        assembly_instructions[assembly_index++] = new_instruction;

    }
    else if(!strcmp(instruction->op, "FUN")){
        if (instruction->arg2 == NULL || instruction->arg2->name == NULL) {
            fprintf(stderr, "[ERRO assembly_t] FUN com arg2 ou nome NULL\n");
            return;
        }

        fflush(stdout);
        
        new_instruction = create_assembly_node(INSTR_TYPE_LABEL, instruction->arg2->name); 
        new_instruction->type_label->is_dynamic = 0;
        add_label(instruction->arg2->name, assembly_index);
        assembly_instructions[assembly_index++] = new_instruction;
        current_func = insert_function(&memory_vector, instruction->arg2->name);
    
        if(!strcmp(instruction->arg2->name, "main")){
            /* Carrega os registradores $fp e $sp com seus valores iniciais */
            new_instruction = create_assembly_node(INSTR_TYPE_I, "ori");
            new_instruction->type_i->rt = $fp;
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = search_function(&memory_vector, "global")->size + get_fp(current_func);
            assembly_instructions[assembly_index++] = new_instruction;

            new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
            new_instruction->type_r->rd = $fp;
            new_instruction->type_r->rs = $fp;
            new_instruction->type_r->rt = $s0;
            assembly_instructions[assembly_index++] = new_instruction;

            fflush(stdout);
            new_instruction = create_assembly_node(INSTR_TYPE_I, "ori");
            new_instruction->type_i->rt = $sp;
            new_instruction->type_i->rs = $zero;
        
            fflush(stdout);
            function_memory_t* globalFunc = search_function(&memory_vector, "global");
            if (globalFunc == NULL) {
                fprintf(stderr, "[ERRO assembly_t] Funcao global nao encontrada\n");
                exit(EXIT_FAILURE);
            }
            fflush(stdout);
            
            new_instruction->type_i->immediate = globalFunc->size + get_sp(current_func);
            fflush(stdout);
            assembly_instructions[assembly_index++] = new_instruction;

            new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
            new_instruction->type_r->rd = $sp;
            new_instruction->type_r->rs = $sp;
            new_instruction->type_r->rt = $s0;
            assembly_instructions[assembly_index++] = new_instruction;

            // Inicia o ponteiro de memoria para os parametros
            // Pilha de parâmetros no final da memória de dados do SO (posição INIT_STACK_PARAMS)
            new_instruction = create_assembly_node(INSTR_TYPE_I, "addi");
            new_instruction->type_i->rt = $pilha;
            new_instruction->type_i->rs = $s0;
            new_instruction->type_i->immediate = INIT_STACK_PARAMS; // valor 499
            assembly_instructions[assembly_index++] = new_instruction;
        }
        else{
            // Guarda o valor de controle para a funcao anterior
            new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
            new_instruction->type_i->rt = $ra;
            new_instruction->type_i->rs = $fp;
            new_instruction->type_i->immediate = get_fp_relation(current_func, get_variable(current_func, "Endereco Retorno")) + instruction->arg3->val;
            assembly_instructions[assembly_index++] = new_instruction;
        }
    }
    else if(!strcmp(instruction->op, "RET")){
        if(!strcmp(current_func->name, "main")) return;
        
        // Acessa o valor de controle da funcao anterior
        new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
        new_instruction->type_i->rt = $temp;
        new_instruction->type_i->rs = $fp;
        new_instruction->type_i->immediate = get_fp_relation(current_func, get_variable(current_func, "Vinculo Controle"));
        assembly_instructions[assembly_index++] = new_instruction;

        // Salva o valor do retorno no frame da funcao anterior
        new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
        new_instruction->type_i->rs = $temp;
        new_instruction->type_i->rt = instruction->arg1->val;
        new_instruction->type_i->immediate = 2; // 2 para avancar para "Valor de Retorno"
        assembly_instructions[assembly_index++] = new_instruction;

    }
    else if(!strcmp(instruction->op, "PARAM")){
        
        if(!strcmp(instruction->arg2->name, "VET")){

            variable_t* var = get_variable(current_func, instruction->arg3->name);

            if(var->type == VAR_ARRAY_ARG){
                new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
                new_instruction->type_i->rt = instruction->arg1->val;
                new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp; // Mas teoricamnete ele eh so $fp <- Mante so para ter certeza
                new_instruction->type_i->immediate = get_fp_relation(current_func, var);
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
                new_instruction->type_i->rs = $pilha;
                new_instruction->type_i->rt = instruction->arg1->val;
                new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
                assembly_instructions[assembly_index++] = new_instruction;
            }
            else{
                new_instruction = create_assembly_node(INSTR_TYPE_I, "addi");
                new_instruction->type_i->rt = instruction->arg1->val;
                new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp;
                new_instruction->type_i->immediate = get_fp_relation(current_func, var);
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
                new_instruction->type_i->rs = $pilha;
                new_instruction->type_i->rt = instruction->arg1->val;
                new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
                assembly_instructions[assembly_index++] = new_instruction;
            }

        }
        else{
            new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
            new_instruction->type_i->rs = $pilha;
            new_instruction->type_i->rt = instruction->arg1->val;
            new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
            assembly_instructions[assembly_index++] = new_instruction;
        }

        insert_variable(search_function(&memory_vector, "parametros"), "Param", VAR_INT);
        
    } 
    else if(!strcmp(instruction->op, "LOAD")){
        if(!instruction->arg3){
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("NULL no argumento 3\n");
            return;
        }

        variable_t* var = get_variable(current_func, instruction->arg2->name);

        if(instruction->arg3->type != ADDR_EMPTY){
            // Load de um indice de um vetor

            if(var->type == VAR_ARRAY){
                // Vetor alocado no escopo dessa funcao
                new_instruction = create_assembly_node(INSTR_TYPE_I, "addi");
                new_instruction->type_i->rt = $temp;
                new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp;
                new_instruction->type_i->immediate = get_fp_relation(current_func, var);
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
                new_instruction->type_r->rd = $temp;
                new_instruction->type_r->rs = $temp;
                new_instruction->type_r->rt = instruction->arg3->val;
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
                new_instruction->type_i->rt = instruction->arg1->val;
                new_instruction->type_i->rs = $temp;
                new_instruction->type_i->immediate = 0;
                assembly_instructions[assembly_index++] = new_instruction;
            }
            else{
                // Vetor passado como parametro
                new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
                new_instruction->type_i->rt = $temp;
                new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp;
                new_instruction->type_i->immediate = get_fp_relation(current_func, var);
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
                new_instruction->type_r->rd = $temp;
                new_instruction->type_r->rs = $temp;
                new_instruction->type_r->rt = instruction->arg3->val;
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
                new_instruction->type_i->rt = instruction->arg1->val;
                new_instruction->type_i->rs = $temp;
                new_instruction->type_i->immediate = 0;
                assembly_instructions[assembly_index++] = new_instruction;
            }

        }
        else{
            // Load de um inteiro
            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rt = instruction->arg1->val;
            new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp;
            new_instruction->type_i->immediate = get_fp_relation(current_func, var);
            assembly_instructions[assembly_index++] = new_instruction;
        }
    }
    else if(!strcmp(instruction->op, "STORE")){
        if(!instruction->arg3){
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("NULL no argumento 3\n");
            return;
        }
        
        variable_t* var = get_variable(current_func, instruction->arg1->name);

        if(instruction->arg3->type != ADDR_EMPTY){
            // Store de um valor em um vetor

            if(var->type == VAR_ARRAY){
                new_instruction = create_assembly_node(INSTR_TYPE_I, "addi");
                new_instruction->type_i->rt = $temp;
                new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp;
                new_instruction->type_i->immediate = get_fp_relation(current_func, var);
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
                new_instruction->type_r->rd = $temp;
                new_instruction->type_r->rs = $temp;
                new_instruction->type_r->rt = instruction->arg3->val;
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
                new_instruction->type_i->rt = instruction->arg2->val;
                new_instruction->type_i->rs = $temp;
                new_instruction->type_i->immediate = 0;
                assembly_instructions[assembly_index++] = new_instruction;
            }
            else{
                new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
                new_instruction->type_i->rt = $temp;
                new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp;
                new_instruction->type_i->immediate = get_fp_relation(current_func, var);
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
                new_instruction->type_r->rd = $temp;
                new_instruction->type_r->rs = $temp;
                new_instruction->type_r->rt = instruction->arg3->val;
                assembly_instructions[assembly_index++] = new_instruction;

                new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
                new_instruction->type_i->rt = instruction->arg2->val;
                new_instruction->type_i->rs = $temp;
                new_instruction->type_i->immediate = 0;
                assembly_instructions[assembly_index++] = new_instruction;
            }
        }
        else{
            new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
            new_instruction->type_i->rt = instruction->arg2->val;
            new_instruction->type_i->rs = (var->is_global) ? $s0 : $fp;
            new_instruction->type_i->immediate = get_fp_relation(current_func, var);
            assembly_instructions[assembly_index++] = new_instruction;
        }
    }
    else if(!strcmp(instruction->op, "GOTO")){
        new_instruction = create_assembly_node(INSTR_TYPE_J, "j");
        new_instruction->type_j->label_immediate = strdup("Label ########");
        sprintf(new_instruction->type_j->label_immediate, "Label %d", instruction->arg1->val);
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "HALT")){
        new_instruction = create_assembly_node(INSTR_TYPE_J, "halt");
        new_instruction->type_j->label_immediate = strdup("$zero");
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "DISP_VAR")){
        // Modo 1: Escreve o valor de um registrador
        new_instruction = create_assembly_node(INSTR_TYPE_I, "disp");
        // O registrador que contém o valor está em arg1
        new_instruction->type_i->rs = instruction->arg1->val;
        new_instruction->type_i->rt = $zero; // rt não é usado neste modo, pode ser $zero
        new_instruction->type_i->immediate = 0; // Imediato é 0 para indicar modo variável
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "DISP_CONST")){
        // Modo 2: Escreve uma mensagem fixa (identificada pelo número em arg1)
        new_instruction = create_assembly_node(INSTR_TYPE_I, "disp");
        new_instruction->type_i->rs = $zero; // rs não é usado neste modo, pode ser $zero
        new_instruction->type_i->rt = $zero; // rt não é usado neste modo
        // O número da mensagem (constante) está em arg1
        new_instruction->type_i->immediate = instruction->arg1->val; // Imediato > 0 indica modo constante (número da mensagem)
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "OS_JUMP_TO")){
        new_instruction = create_assembly_node(INSTR_TYPE_I, "os_jump_to");
        new_instruction->type_i->rs = $k0;  // Usa o registrador $k0 para o endereço de salto
        new_instruction->type_i->rt = $zero;
        new_instruction->type_i->immediate = 0;
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "NO_OP")){
        new_instruction = create_assembly_node(INSTR_TYPE_R, "no_op");
        new_instruction->type_r->rs = $zero;
        new_instruction->type_r->rt = $zero;
        new_instruction->type_r->rd = $zero;
        new_instruction->type_r->shamt = 0;
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "OS_SET_DM_BASE")){
        // Move o valor do registrador para $s0 usando add
        new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
        new_instruction->type_r->rd = $s0;
        new_instruction->type_r->rs = instruction->arg1->val;
        new_instruction->type_r->rt = $zero;
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "OS_SET_IM_BASE")){
        // Move o valor do registrador para $s1 usando add
        new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
        new_instruction->type_r->rd = $s1;
        new_instruction->type_r->rs = instruction->arg1->val;
        new_instruction->type_r->rt = $zero;
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "OS_SAVE_RETURN")){
        // Instrução os_save_return: salva o ponto de retorno do SO
        new_instruction = create_assembly_node(INSTR_TYPE_I, "os_save_return");
        new_instruction->type_i->rs = $zero;
        new_instruction->type_i->rt = $zero;
        new_instruction->type_i->immediate = 0;
        assembly_instructions[assembly_index++] = new_instruction;
    }
    // Instrução para salvar contexto
    else if(!strcmp(instruction->op, "OS_SAVE_CONTEXT")){
        for (int i = 0; i < 25 ; i++) {
            new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
            new_instruction->type_i->rs = $s0; // Base de dados do SO
            new_instruction->type_i->rt = i;   // Registrador a ser salvo
            new_instruction->type_i->immediate = INIT_CONTEXT_SWITCH + i; // valor 467
            assembly_instructions[assembly_index++] = new_instruction;
        }
        /* Salta o registrador $25 que é setado pelo SO o PC que deverá saltar. */
        for (int i = 26; i < 32 ; i++) {
            new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
            new_instruction->type_i->rs = $s0; // Base de dados do SO
            new_instruction->type_i->rt = i;   // Registrador a ser salvo
            new_instruction->type_i->immediate = INIT_CONTEXT_SWITCH + i; // valor 467
            assembly_instructions[assembly_index++] = new_instruction;
        }
    }
    // Instrução para restaurar contexto
    else if(!strcmp(instruction->op, "OS_RESTORE_CONTEXT")){
        for (int i = 0; i < 25 ; i++) {
            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rs = $s0; // Base de dados do SO
            new_instruction->type_i->rt = i;   // Registrador a ser restaurado
            new_instruction->type_i->immediate = INIT_CONTEXT_SWITCH + i; // valor 467
            assembly_instructions[assembly_index++] = new_instruction;
        }
        /* Salta o registrador $25 que é setado pelo SO o PC que deverá saltar. */
        for (int i = 26; i < 32 ; i++) {
            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rs = $s0; // Base de dados do SO
            new_instruction->type_i->rt = i;   // Registrador a ser restaurado
            new_instruction->type_i->immediate = INIT_CONTEXT_SWITCH + i; // valor 467
            assembly_instructions[assembly_index++] = new_instruction;
        }
    }
    else if(!strcmp(instruction->op, "SET_INTERR_TIMER")){
        new_instruction = create_assembly_node(INSTR_TYPE_I, "set_interr_timer");
        new_instruction->type_i->rs = $zero;
        new_instruction->type_i->rt = $zero;
        new_instruction->type_i->immediate = instruction->arg1->val; // Valor direto da constante
        assembly_instructions[assembly_index++] = new_instruction;
    }
    // os_set_pc deve carregar o valor de um registrador para o registrador $k0
    else if(!strcmp(instruction->op, "OS_SET_PC")){
        new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
        new_instruction->type_r->rd = $k0;
        new_instruction->type_r->rs = instruction->arg1->val; // Registrador com o valor do PC
        new_instruction->type_r->rt = $zero;
        assembly_instructions[assembly_index++] = new_instruction;
    }
    else if(!strcmp(instruction->op, "CALL")){
        if(!instruction->arg3){
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("NULL no argumento 3\n");
            return;
        }
                
        if(!strcmp(instruction->arg1->name, "output")){
            
            delete_temp(search_function(&memory_vector, "parametros")); // Apaga os temporarios usados na chamada

            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rt = $temp;
            new_instruction->type_i->rs = $pilha;
            new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
            assembly_instructions[assembly_index++] = new_instruction;

            // Mostra o valor do $temp para o usuario
            new_instruction = create_assembly_node(INSTR_TYPE_I, "out");
            new_instruction->type_i->rs = $temp;
            new_instruction->type_i->rt = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;

            return; // Nao precisa fazer mais nada
        }
        else if(!strcmp(instruction->arg1->name, "input")){
            // Como essa funcao nao tem param, basta colocar o input no registrador
            new_instruction = create_assembly_node(INSTR_TYPE_I, "in");
            new_instruction->type_i->rt = instruction->arg3->val;
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;
            return; // Nao precisa fazer mais nada
        }
        else if(!strcmp(instruction->arg1->name, "get_pc")){
            // Coloca o valor do PC no registrador passado como argumento
            new_instruction = create_assembly_node(INSTR_TYPE_I, "get_pc");
            new_instruction->type_i->rt = instruction->arg3->val;
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;
            return;
        }
        // Instrução "get_interr_type" com opcode especial
        else if(!strcmp(instruction->arg1->name, "get_interr_type")){
            new_instruction = create_assembly_node(INSTR_TYPE_I, "get_interr_type");
            new_instruction->type_i->rt = instruction->arg3->val; // Registrador para armazenar o tipo de interrupção
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;
            return;
        }
        // Instrução draw_pixel
        else if(!strcmp(instruction->arg1->name, "draw_pixel")){
            delete_temp(search_function(&memory_vector, "parametros"));

            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rt = $temp2;
            new_instruction->type_i->rs = $pilha;
            new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
            assembly_instructions[assembly_index++] = new_instruction;

            delete_temp(search_function(&memory_vector, "parametros"));

            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rt = $temp;
            new_instruction->type_i->rs = $pilha;
            new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
            assembly_instructions[assembly_index++] = new_instruction;

            new_instruction = create_assembly_node(INSTR_TYPE_I, "draw_pixel");
            new_instruction->type_i->rt = $temp2;
            new_instruction->type_i->rs = $temp;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;

            return;
        }
        // Instrução keyboard_input
        else if(!strcmp(instruction->arg1->name, "keyboard_input")){
            new_instruction = create_assembly_node(INSTR_TYPE_I, "keyboard_input");
            new_instruction->type_i->rt = instruction->arg3->val; // Indica o valor do teclado
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;
            
            return;
        }
        // Instrução uart_send: envia 1 byte pela UART (FPGA -> Arduino).
        // Segue o padrão do "output": carrega o parâmetro em $temp e o transmite.
        // No hardware, o byte enviado vem de rs (br_dado1[7:0]).
        else if(!strcmp(instruction->arg1->name, "uart_send")){
            delete_temp(search_function(&memory_vector, "parametros")); // Apaga o temporario usado na chamada

            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rt = $temp;
            new_instruction->type_i->rs = $pilha;
            new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
            assembly_instructions[assembly_index++] = new_instruction;

            new_instruction = create_assembly_node(INSTR_TYPE_I, "uart_send");
            new_instruction->type_i->rs = $temp;  // byte a transmitir
            new_instruction->type_i->rt = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;

            return;
        }
        // Instrução uart_tx_ready: retorna 1 se a TX da UART está livre.
        else if(!strcmp(instruction->arg1->name, "uart_tx_ready")){
            new_instruction = create_assembly_node(INSTR_TYPE_I, "uart_tx_ready");
            new_instruction->type_i->rt = instruction->arg3->val; // Registrador que recebe o status
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;

            return;
        }
        // Instrução uart_rx_available: retorna 1 se há um byte recebido (não consumido).
        else if(!strcmp(instruction->arg1->name, "uart_rx_available")){
            new_instruction = create_assembly_node(INSTR_TYPE_I, "uart_rx_available");
            new_instruction->type_i->rt = instruction->arg3->val; // Registrador que recebe o status
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;

            return;
        }
        // Instrução uart_receive: retorna o byte recebido pela UART (e o consome).
        else if(!strcmp(instruction->arg1->name, "uart_receive")){
            new_instruction = create_assembly_node(INSTR_TYPE_I, "uart_receive");
            new_instruction->type_i->rt = instruction->arg3->val; // Registrador que recebe o byte
            new_instruction->type_i->rs = $zero;
            new_instruction->type_i->immediate = 0;
            assembly_instructions[assembly_index++] = new_instruction;

            return;
        }

        for(int i = instruction->arg2->val; i > 0; i--) {
            // Salva o valor do param no $temp para ser usado no output			
            delete_temp(search_function(&memory_vector, "parametros")); // Apaga os temporarios usados na chamada

            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rt = $temp;
            new_instruction->type_i->rs = $pilha;
            new_instruction->type_i->immediate = search_function(&memory_vector, "parametros")->size;
            assembly_instructions[assembly_index++] = new_instruction;

            new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
            new_instruction->type_i->rs = $sp;
            new_instruction->type_i->rt = $temp;
            new_instruction->type_i->immediate = i;
            assembly_instructions[assembly_index++] = new_instruction;
        }

        // Armazena o valor de $fp dessa funcao em $temp para poder ser armazenado no novo frame
        new_instruction = create_assembly_node(INSTR_TYPE_R, "add");
        new_instruction->type_r->rt = $zero;
        new_instruction->type_r->rs = $fp;
        new_instruction->type_r->rd = $temp;
        assembly_instructions[assembly_index++] = new_instruction;

        // Faz com que $temp aponte para "Vinculo Controle" para poder ser armazenado no novo frame
        new_instruction = create_assembly_node(INSTR_TYPE_I, "addi");
        new_instruction->type_i->rt = $temp;
        new_instruction->type_i->rs = $temp;
        new_instruction->type_i->immediate = get_fp_relation(current_func, get_variable(current_func, "Vinculo Controle"));
        assembly_instructions[assembly_index++] = new_instruction;

        // Armazena o valor de controle no seu respectivo local no novo frame
        new_instruction = create_assembly_node(INSTR_TYPE_I, "sw");
        new_instruction->type_i->rt = $temp;
        new_instruction->type_i->rs = $sp;
        new_instruction->type_i->immediate = instruction->arg2->val + 1;
        assembly_instructions[assembly_index++] = new_instruction;

        // Incrementa o valor de $fp e $sp para o novo frame da funcao
        new_instruction = create_assembly_node(INSTR_TYPE_I, "addi");
        new_instruction->type_i->rt = $fp;
        new_instruction->type_i->rs = $fp;
        new_instruction->type_i->immediate = get_sp(current_func) + 1;
        assembly_instructions[assembly_index++] = new_instruction;

        function_memory_t* funcaoChamada = search_function(&memory_vector, instruction->arg1->name);

        new_instruction = create_assembly_node(INSTR_TYPE_I, "addi");
        new_instruction->type_i->rt = $sp;
        new_instruction->type_i->rs = $sp;
        new_instruction->type_i->immediate = get_sp(funcaoChamada) + 1;
        assembly_instructions[assembly_index++] = new_instruction;

        // Pulando para a funcao chamada
        new_instruction = create_assembly_node(INSTR_TYPE_J, "jal");
        new_instruction->type_j->label_immediate = strdup(instruction->arg1->name);
        assembly_instructions[assembly_index++] = new_instruction;

        // Volta os valores de $fp
        new_instruction = create_assembly_node(INSTR_TYPE_I, "subi");
        new_instruction->type_i->rt = $fp;
        new_instruction->type_i->rs = $fp;
        new_instruction->type_i->immediate = get_sp(current_func) + 1;
        assembly_instructions[assembly_index++] = new_instruction;

        // Volta os valores de $sp
        new_instruction = create_assembly_node(INSTR_TYPE_I, "subi");
        new_instruction->type_i->rt = $sp;
        new_instruction->type_i->rs = $sp;
        new_instruction->type_i->immediate = get_sp(funcaoChamada) + 1;
        assembly_instructions[assembly_index++] = new_instruction;

        if(instruction->arg3->type != ADDR_EMPTY){
            // Armazena o valor do retorno da funcao anterior no registrador passado como argumento
            new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
            new_instruction->type_i->rt = instruction->arg3->val;
            new_instruction->type_i->rs = $fp;
            new_instruction->type_i->immediate = get_fp_relation(current_func, get_variable(current_func, "Valor Retorno"));
            assembly_instructions[assembly_index++] = new_instruction;
        }
    } 
    else if(!strcmp(instruction->op, "END")){
        if(!strcmp(instruction->arg1->name, "main")){
            return; // Nao precisa fazer mais nada, ja que a proxima instruction eh o HALT
        }
        
        // Restaura o valor de $ra da funcao anterior
        new_instruction = create_assembly_node(INSTR_TYPE_I, "lw");
        new_instruction->type_i->rs = $fp;
        new_instruction->type_i->rt = $ra;
        new_instruction->type_i->immediate = get_fp_relation(current_func, get_variable(current_func, "Endereco Retorno"));
        assembly_instructions[assembly_index++] = new_instruction;
    
        // Pula para a instruction que fez a chamada da funcao
        new_instruction = create_assembly_node(INSTR_TYPE_R, "jr");
        new_instruction->type_r->rs = $ra;
        new_instruction->type_r->rd = $zero;
        new_instruction->type_r->rt = $zero;
        assembly_instructions[assembly_index++] = new_instruction;

    }
    else{
        printf(ANSI_COLOR_RED);
        printf("Erro: ");
        printf(ANSI_COLOR_RESET);
        printf("Instrucao nao reconhecida (%s)\n", instruction->op);
    }

}
