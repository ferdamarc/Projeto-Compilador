#include "synthesis.h"

unsigned int get_opcode(char* nome, instruction_type_t tipo) {
    int opcode = -1;

    if (tipo == INSTR_TYPE_R) {
        opcode = 0b000000;
    }
    /* Load/Store Instructions */
    else if (strcmp(nome, "lw") == 0)    opcode = 0b100011;
    else if (strcmp(nome, "sw") == 0)    opcode = 0b101011;
    
    /* Immediate Arithmetic and Logical Instructions */
    else if (strcmp(nome, "addi") == 0)  opcode = 0b001000;
    else if (strcmp(nome, "subi") == 0)  opcode = 0b001001;
    else if (strcmp(nome, "andi") == 0)  opcode = 0b001100;
    else if (strcmp(nome, "ori") == 0)   opcode = 0b001101;
    else if (strcmp(nome, "xori") == 0)  opcode = 0b001110;
    else if (strcmp(nome, "slti") == 0)  opcode = 0b001010;
    
    /* Branch Instructions */
    else if (strcmp(nome, "beq") == 0)   opcode = 0b000100;
    else if (strcmp(nome, "bne") == 0)   opcode = 0b000101;
    
    /* Jump Instructions */
    else if (strcmp(nome, "j") == 0)     opcode = 0b000010;
    else if (strcmp(nome, "jal") == 0)   opcode = 0b000011;
    
    /* I/O Instructions (Custom) */
    else if (strcmp(nome, "in") == 0)    opcode = 0b011111;
    else if (strcmp(nome, "out") == 0)   opcode = 0b011110;
    else if (strcmp(nome, "disp") == 0)  opcode = 0b011101;
    
    /* Operating System Instructions (Custom) */
    else if (strcmp(nome, "os_jump_to") == 0)       opcode = 0b010010;
    else if (strcmp(nome, "os_save_return") == 0)   opcode = 0b010011;
    else if (strcmp(nome, "get_pc") == 0)           opcode = 0b010100;
    else if (strcmp(nome, "set_interr_timer") == 0) opcode = 0b010101;
    else if (strcmp(nome, "get_interr_type") == 0)  opcode = 0b010110;

    /* Keyboard Input Instruction */
    else if (strcmp(nome, "keyboard_input") == 0)   opcode = 0b010111;

    /* Draw Pixel Instruction */
    else if (strcmp(nome, "draw_pixel") == 0)   opcode = 0b001111;

    /* UART Instructions (TX) */
    else if (strcmp(nome, "uart_send") == 0)     opcode = 0b011000;
    else if (strcmp(nome, "uart_tx_ready") == 0) opcode = 0b011011;

    /* UART Instructions (RX) */
    else if (strcmp(nome, "uart_receive") == 0)      opcode = 0b011001;
    else if (strcmp(nome, "uart_rx_available") == 0) opcode = 0b011010;

    /* System Control */
    else if (strcmp(nome, "halt") == 0)  opcode = 0b111111;
    
    return opcode;
}


unsigned int get_funct(char* nome) {
    int funct = -1;

    /* Arithmetic Operations */
    if (strcmp(nome, "add") == 0)       funct = 0b100000;
    else if (strcmp(nome, "sub") == 0)  funct = 0b100010;
    else if (strcmp(nome, "mult") == 0) funct = 0b011000;
    else if (strcmp(nome, "div") == 0)  funct = 0b011010;
    
    /* Logical Operations */
    else if (strcmp(nome, "and") == 0)  funct = 0b100100;
    else if (strcmp(nome, "or") == 0)   funct = 0b100101;
    else if (strcmp(nome, "xor") == 0)  funct = 0b101101;
    else if (strcmp(nome, "nor") == 0)  funct = 0b100111;
    
    /* Shift Operations */
    else if (strcmp(nome, "sll") == 0)  funct = 0b000000;
    else if (strcmp(nome, "srl") == 0)  funct = 0b000010;
    
    /* Jump Operations */
    else if (strcmp(nome, "jr") == 0)   funct = 0b001000;
    else if (strcmp(nome, "jalr") == 0) funct = 0b001001;
    
    /* Comparison */
    else if (strcmp(nome, "slt") == 0)  funct = 0b101010;
    
    return funct;
}

unsigned int get_address(char* label) {
    return get_label_address(label);
}


binary_r_t* binarioNop() {
    binary_r_t* bin = (binary_r_t*)malloc(sizeof(binary_r_t));
    if (bin == NULL) {
        fprintf(stderr, "Error: Failed to allocate NOP instruction\n");
        exit(EXIT_FAILURE);
    }
    
    bin->opcode = 0;
    bin->rs = $zero;
    bin->rt = $zero;
    bin->rd = $zero;
    bin->shamt = 0;
    bin->funct = 0b100000;  /* ADD function code */
    
    return bin;
}


binary_r_t* binarioR(assembly_t* instruction) {
    /* Handle NOP instruction specially */
    if (strcmp(instruction->type_r->name, "no_op") == 0) {
        return binarioNop();
    }
    
    binary_r_t* bin = (binary_r_t*)malloc(sizeof(binary_r_t));
    if (bin == NULL) {
        fprintf(stderr, "Error: Failed to allocate R-type instruction\n");
        exit(EXIT_FAILURE);
    }
    
    bin->opcode = get_opcode(instruction->type_r->name, instruction->type);
    bin->rs = instruction->type_r->rs;
    bin->rt = instruction->type_r->rt;
    bin->rd = instruction->type_r->rd;
    bin->shamt = instruction->type_r->shamt;
    bin->funct = get_funct(instruction->type_r->name);
    
    return bin;
}


binary_i_t* binarioI(assembly_t* instruction) {
    binary_i_t* bin = (binary_i_t*)malloc(sizeof(binary_i_t));
    if (bin == NULL) {
        fprintf(stderr, "Error: Failed to allocate I-type instruction\n");
        exit(EXIT_FAILURE);
    }
    
    bin->opcode = get_opcode(instruction->type_i->name, instruction->type);
    bin->rs = instruction->type_i->rs;
    bin->rt = instruction->type_i->rt;
    
    /* Branch instructions use label addresses */
    if (strcmp(instruction->type_i->name, "bne") == 0 || 
        strcmp(instruction->type_i->name, "beq") == 0) {
        /* Convert label number to label string */
        char label[26];
        sprintf(label, "Label %d", instruction->type_i->label);
        bin->immediate = get_address(label);
    }
    else {
        /* Other I-type instructions use immediate values */
        bin->immediate = instruction->type_i->immediate;
    }

    return bin;
}


binary_j_t* binarioJ(assembly_t* instruction) {
    binary_j_t* bin = (binary_j_t*)malloc(sizeof(binary_j_t));
    if (bin == NULL) {
        fprintf(stderr, "Error: Failed to allocate J-type instruction\n");
        exit(EXIT_FAILURE);
    }
    
    bin->opcode = get_opcode(instruction->type_j->name, instruction->type);
    bin->address = get_address(instruction->type_j->label_immediate);
    
    return bin;
}		


void printBits(size_t const size, void const * const ptr, FILE* arquivo) {
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;

    /* Iterate through bytes in reverse order (big-endian) */
    for (int i = size - 1; i >= 0; i--) {
        /* Print each bit of the byte */
        for (int j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            fprintf(arquivo, "%u", byte);
        }
    }
}


void binary(FILE* arquivo) {
    binary_i_t* binI;
    binary_j_t* binJ;
    binary_r_t* binR;
    
    for (int i = 0; i < assembly_index; i++) {
        if (assembly_instructions[i] == NULL) {
            fprintf(stderr, "[ERRO BINARIO] Instrucao NULL no indice %d\n", i);
            continue;
        }
        
        switch (assembly_instructions[i]->type) {
            case INSTR_TYPE_R:
                binR = binarioR(assembly_instructions[i]);
                printBits(sizeof(*binR), binR, arquivo);
                free(binR);
                break;
                
            case INSTR_TYPE_I:
                binI = binarioI(assembly_instructions[i]);
                printBits(sizeof(*binI), binI, arquivo);
                free(binI);
                break;
                
            case INSTR_TYPE_J:
                binJ = binarioJ(assembly_instructions[i]);
                printBits(sizeof(*binJ), binJ, arquivo);
                free(binJ);
                break;
                
            case INSTR_TYPE_LABEL:
                /* Labels become NOP instructions in binary */
                binR = binarioNop();
                printBits(sizeof(*binR), binR, arquivo);
                free(binR);
                break;
        }
        fprintf(arquivo, "\n");
    }
}


void binary_debug(FILE* arquivo) {
    binary_i_t* binI;
    binary_j_t* binJ;
    binary_r_t* binR;
    
    for (int i = 0; i < assembly_index; i++) {
        fprintf(arquivo, "%d:\t", i);
        
        switch (assembly_instructions[i]->type) {
            case INSTR_TYPE_R:
                binR = binarioR(assembly_instructions[i]);
                printBits(sizeof(*binR), binR, arquivo);
                fprintf(arquivo, " : %s", assembly_instructions[i]->type_r->name);
                free(binR);
                break;
                
            case INSTR_TYPE_I:
                binI = binarioI(assembly_instructions[i]);
                printBits(sizeof(*binI), binI, arquivo);
                fprintf(arquivo, " : %s", assembly_instructions[i]->type_i->name);
                free(binI);
                break;
                
            case INSTR_TYPE_J:
                binJ = binarioJ(assembly_instructions[i]);
                printBits(sizeof(*binJ), binJ, arquivo);
                fprintf(arquivo, " : %s", assembly_instructions[i]->type_j->name);
                free(binJ);
                break;
                
            case INSTR_TYPE_LABEL:
                binR = binarioNop();
                printBits(sizeof(*binR), binR, arquivo);
                fprintf(arquivo, " : nop");
                free(binR);
                break;
        }
        fprintf(arquivo, "\n");
    }
}