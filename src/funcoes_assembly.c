#include "synthesis.h"

/* Array of assembly instructions */
assembly_t ** assembly_instructions = NULL;

/* Current index in the assembly instruction array */
int assembly_index = 0;

void initialize_assembly() {
    /* Allocate instruction array - TODO: Change to dynamic allocation */
    assembly_instructions = (assembly_t **)malloc(sizeof(assembly_t*) * MAX_ASSEMBLY);
    
    if (assembly_instructions == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for assembly instructions\n");
        exit(EXIT_FAILURE);
    }

    /* Initialize all instruction pointers to NULL */
    for (int i = 0; i < MAX_INSTRUCTION; i++) {
        assembly_instructions[i] = NULL;
    }

    assembly_index = 0;

    /* Initialize supporting systems */
    initialize_labels();
    initialize_memory(&memory_vector);
    current_func = memory_vector.funcs;
}


assembly_t * create_assembly_node(instruction_type_t tipo, char *nome) {
    
    assembly_t * novoNoAssembly = (assembly_t *)malloc(sizeof(assembly_t));
    if (novoNoAssembly == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for assembly instruction\n");
        exit(EXIT_FAILURE);
    }
    
    novoNoAssembly->type = tipo;

    switch (tipo) {
        case INSTR_TYPE_R:
            novoNoAssembly->type_r = (r_type_t *)malloc(sizeof(r_type_t));
            if (novoNoAssembly->type_r == NULL) {
                fprintf(stderr, "Error: Failed to allocate R-type instruction\n");
                free(novoNoAssembly);
                exit(EXIT_FAILURE);
            }
            novoNoAssembly->type_r->name = nome;
            novoNoAssembly->type_r->rd = -1;
            novoNoAssembly->type_r->rs = -1;
            novoNoAssembly->type_r->rt = -1;
            novoNoAssembly->type_r->shamt = 0;
            break;

        case INSTR_TYPE_I:
            novoNoAssembly->type_i = (i_type_t *)malloc(sizeof(i_type_t));
            if (novoNoAssembly->type_i == NULL) {
                fprintf(stderr, "Error: Failed to allocate I-type instruction\n");
                free(novoNoAssembly);
                exit(EXIT_FAILURE);
            }
            novoNoAssembly->type_i->name = nome;
            novoNoAssembly->type_i->rs = -1;
            novoNoAssembly->type_i->rt = -1;
            novoNoAssembly->type_i->immediate = -1;
            novoNoAssembly->type_i->label = -1;
            break;

        case INSTR_TYPE_J:
            novoNoAssembly->type_j = (j_type_t *)malloc(sizeof(j_type_t));
            if (novoNoAssembly->type_j == NULL) {
                fprintf(stderr, "Error: Failed to allocate J-type instruction\n");
                free(novoNoAssembly);
                exit(EXIT_FAILURE);
            }
            novoNoAssembly->type_j->name = nome;
            novoNoAssembly->type_j->label_immediate = NULL;
            break;
        
        case INSTR_TYPE_LABEL:
            novoNoAssembly->type_label = (label_type_t *)malloc(sizeof(label_type_t));
            if (novoNoAssembly->type_label == NULL) {
                fprintf(stderr, "Error: Failed to allocate Label-type instruction\n");
                free(novoNoAssembly);
                exit(EXIT_FAILURE);
            }

            novoNoAssembly->type_label->name = nome;
            novoNoAssembly->type_label->address = -1;
            novoNoAssembly->type_label->is_dynamic = -1;
            break;
    }

    return novoNoAssembly;
}


void free_assembly() {
    for (int i = 0; i < assembly_index; i++) {
        switch (assembly_instructions[i]->type) {
            case INSTR_TYPE_R:
                free(assembly_instructions[i]->type_r);
                break;
                
            case INSTR_TYPE_I:
                free(assembly_instructions[i]->type_i);
                break;
                
            case INSTR_TYPE_J:
                free(assembly_instructions[i]->type_j);
                break;
                
            case INSTR_TYPE_LABEL:
                /* Free label name if it was dynamically allocated */
                if (assembly_instructions[i]->type_label->is_dynamic == 1) {
                    free(assembly_instructions[i]->type_label->name);
                }
                free(assembly_instructions[i]->type_label);
                break;
        }
        free(assembly_instructions[i]);
    }
    free(assembly_instructions);
}


void tipo_reg(int reg) {
    switch (reg) {
        case $zero:
            fprintf(output_assembly_file, "$zero");
            break;
        
        case $fp:
            fprintf(output_assembly_file, "$fp");
            break;

        case $sp:
            fprintf(output_assembly_file, "$sp");
            break;

        case $ra:
            fprintf(output_assembly_file, "$ra");
            break;

        case $temp:
            fprintf(output_assembly_file, "$temp");
            break;
        
        case $pilha:
            fprintf(output_assembly_file, "$pilha");
            break;

        case $k0:
            fprintf(output_assembly_file, "$k0");
            break;
        
        case $s0:
            fprintf(output_assembly_file, "$s0");
            break;
        
        case $s1:
            fprintf(output_assembly_file, "$s1");
            break;
            
        default:
            /* General purpose temporary registers */
            fprintf(output_assembly_file, "$t%d", reg);
            break;
    }
}


void print_assembly() {
    i_type_t * tipoI = NULL;
    r_type_t * tipoR = NULL;
    j_type_t * tipoJ = NULL;
    label_type_t * tipoLabel = NULL;

    fprintf(output_assembly_file, "============== Assembly ==============\n");
    
    for (int i = 0; i < assembly_index; i++) {
        /* Print line number with padding */
        fprintf(output_assembly_file, "%d: ", i);
        if (i < 10) {
            fprintf(output_assembly_file, " ");
        }
        
        /* Format instruction based on type */
        switch (assembly_instructions[i]->type) {
            case INSTR_TYPE_I:
                tipoI = assembly_instructions[i]->type_i;		
                fprintf(output_assembly_file, "\t%s ", tipoI->name);
                tipo_reg(tipoI->rt);
                fprintf(output_assembly_file, " ");

                /* Load/Store instructions use offset(base) format */
                if (strcmp(tipoI->name, "lw") == 0 || strcmp(tipoI->name, "sw") == 0) {
                    fprintf(output_assembly_file, "%d(", tipoI->immediate);
                    tipo_reg(tipoI->rs);
                    fprintf(output_assembly_file, ")\n");
                }
                else {
                    /* Other I-type instructions: rt, rs, immediate/label */
                    tipo_reg(tipoI->rs);
                    fprintf(output_assembly_file, " ");
                    
                    if (tipoI->label != -1) {
                        fprintf(output_assembly_file, "Label %d\n", tipoI->label);
                    }
                    else {
                        fprintf(output_assembly_file, "%d\n", tipoI->immediate);
                    }
                }
                break;
                
            case INSTR_TYPE_R:
                tipoR = assembly_instructions[i]->type_r;
                fprintf(output_assembly_file, "\t%s ", tipoR->name);
                tipo_reg(tipoR->rd);
                fprintf(output_assembly_file, " ");
                tipo_reg(tipoR->rs);
                fprintf(output_assembly_file, " ");
                tipo_reg(tipoR->rt);
                fprintf(output_assembly_file, "\n");
                break;
                
            case INSTR_TYPE_J:
                tipoJ = assembly_instructions[i]->type_j;
                fprintf(output_assembly_file, "\t%s %s\n", 
                    tipoJ->name, tipoJ->label_immediate);
                break;
                
            case INSTR_TYPE_LABEL:
                tipoLabel = assembly_instructions[i]->type_label;
                fprintf(output_assembly_file, "%s:\n", tipoLabel->name);
                break;
        }
    }	
}
