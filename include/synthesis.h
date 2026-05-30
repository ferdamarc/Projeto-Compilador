#ifndef _SYNTHESIS_H_
#define _SYNTHESIS_H_ 1

#include "analysis.h"      /* Need AST and symbol table types */
#include "memory_layout.h" /* SO_INTERVAL, START_IM, PROGRAM_INTERVAL, MAX_PROGS, etc. */

/*******************************************************************************
* CONSTANTS FOR OS
******************************************************************************/

#define MAX_INSTRUCTION 10000         /* Maximum instructions in intermediate code */
#define MAX_ASSEMBLY 10000            /* Maximum assembly instructions */
#define MAX_CHAR_NOME 5               /* Maximum characters in mnemonic */
#define SEGMENTATION_SIZE_DM 500      /* Data memory segmentation size */

/* Stack/context constants derived from data-memory segmentation. */
#define INIT_STACK_PARAMS    (SEGMENTATION_SIZE_DM-1)  /* Parameter stack start */
#define INIT_CONTEXT_SWITCH  (INIT_STACK_PARAMS-32)    /* Context switch start */

/* Register Definitions */
#define $zero       31           /* Zero register (always 0) */
#define $ra         30           /* Return address register */
#define $fp         29           /* Frame pointer */
#define $sp         28           /* Stack pointer */
#define $temp       27           /* Temporary register */
#define $pilha      26           /* Stack register */
#define $k0         25           /* Kernel register 0 */
#define $s1         24           /* Saved register 1 */
#define $s0         23           /* Saved register 0 */
#define $temp2      22           /* Temporary register 2 */


/* Address Types (Intermediate Code) */
typedef enum {
    ADDR_EMPTY,     /* Empty/unused address */
    ADDR_INT_CONST, /* Integer constant or register */
    ADDR_STRING     /* String (variable name, label) */
} address_type_t;

/* Assembly Instruction Types */
typedef enum {
    INSTR_TYPE_R,     /* Register-type (3 registers, shift) */
    INSTR_TYPE_I,     /* Immediate-type (2 registers, immediate) */
    INSTR_TYPE_J,     /* Jump-type (label/address) */
    INSTR_TYPE_LABEL  /* Label (pseudo-instruction) */
} instruction_type_t;

/* Memory Variable Types */
typedef enum {
    VAR_INT,        /* Integer variable */
    VAR_ARRAY,      /* Array variable */
    VAR_INT_ARG,    /* Integer argument */
    VAR_ARRAY_ARG,  /* Array argument */
    VAR_CONTROL,    /* Control linkage */
    VAR_RETURN,     /* Return address/value */
    VAR_TEMP        /* Temporary variable */
} var_type_t;


typedef struct address {
    address_type_t type;    /* Type of address */
    int val;                /* Numeric value (for ADDR_INT_CONST) */
    int is_register;        /* 0=number, 1=register, 2=label */
    char* name;             /* String value (for ADDR_STRING type) */
} address_t;

typedef struct instruction {
    char* op;           /* Operation/opcode */
    address_t* arg1;    /* First argument */
    address_t* arg2;    /* Second argument */
    address_t* arg3;    /* Third argument (usually destination) */
} instruction_t;


typedef struct r_type {
    char *name;     /* Instruction mnemonic */
    int rd;         /* Destination register */
    int rt;         /* Target register */
    int rs;         /* Source register */
    int shamt;      /* Shift amount */
} r_type_t;

typedef struct i_type {
    char *name;      /* Instruction mnemonic */
    int rs;          /* Source register */
    int rt;          /* Target register */
    int immediate;   /* Immediate value */
    int label;       /* Label number (for branches) */
} i_type_t;

typedef struct j_type {
    char *name;            /* Instruction mnemonic */
    char *label_immediate; /* Label name */
} j_type_t;

typedef struct label_type {
    int is_dynamic;  /* 1 if name is dynamically allocated */
    char *name;      /* Label name */
    int address;     /* Address/position of label */
} label_type_t;

typedef struct assembly {
    instruction_type_t type;  /* Instruction type */
    i_type_t *type_i;         /* I-type fields (if applicable) */
    r_type_t *type_r;         /* R-type fields (if applicable) */
    j_type_t *type_j;         /* J-type fields (if applicable) */
    label_type_t *type_label; /* Label fields (if applicable) */
} assembly_t;


typedef struct label {
    char* id;           /* Label identifier/name */
    int address;        /* Memory address */
    struct label *next; /* Next label */
} label_t;

typedef struct label_vector {
    label_t *vector;  /* First label in list */
    int size;         /* Number of labels */
} label_vector_t;


typedef struct variable {
    var_type_t type;        /* Variable type */
    int index;              /* Index/position in memory frame */
    int is_global;          /* 1 if global, 0 if local */
    char *name;             /* Variable name */
    struct variable *next;  /* Next variable */
} variable_t;

typedef struct function_memory {
    int size;                         /* Number of variables */
    char* name;                       /* Function name */
    struct function_memory *next;     /* Next function frame */
    variable_t *var_table;            /* Variable list */
} function_memory_t;

typedef struct {
    int size;                  /* Number of function frames */
    function_memory_t *funcs;  /* First function frame */
} memory_t;


typedef struct {
    unsigned int funct:6;     /* Function code */
    unsigned int shamt:5;     /* Shift amount */
    unsigned int rd:5;        /* Destination register */
    unsigned int rt:5;        /* Source/target register 2 */
    unsigned int rs:5;        /* Source register 1 */
    unsigned int opcode:6;    /* Operation code */
} binary_r_t;

typedef struct {
    unsigned int immediate:16; /* Immediate value or offset */
    unsigned int rt:5;         /* Target register */
    unsigned int rs:5;         /* Source register */
    unsigned int opcode:6;     /* Operation code */
} binary_i_t;

typedef struct {
    unsigned int address:26;   /* Jump target address */
    unsigned int opcode:6;     /* Operation code */
} binary_j_t;


extern instruction_t** intermediate_code;  /* Array of intermediate instructions */
extern int num_reg;                        /* Current register number */
extern int array_index;                    /* Index in instruction array */
extern int num_label;                      /* Current label number */
extern char func_name[MAXLEXEMA];          /* Current function name */


extern assembly_t **assembly_instructions; /* Array of assembly instructions */
extern int assembly_index;                 /* Index in assembly array */


extern label_vector_t *label_vector;  /* Global label vector */


extern memory_t memory_vector;             /* Global memory structure */
extern function_memory_t* current_func;    /* Current active function */
extern function_memory_t* global_func;     /* Global scope frame */


void initialize_vector();
void deallocate_vector();
void initialize_registers();
int add_var_register(char* var_name, char* scope);
int add_temp_register();
int search_var_register(char* var_name, char* scope);
void show_registers();
int discard_register();
int check_registers(char *lexeme, char* scope, int is_temp);
void create_intermediate_code(ast_node_ptr syntax_tree, symbol_item_ptr hash_table[], int boolean);
void print_intermediate_code();
instruction_t* create_instruction(char* op);
address_t* create_address(address_type_t type, int val, char* name, int is_register);
void add_instruction(instruction_t* instruction);
instruction_t* create_label_instr(int label_num);
instruction_t* create_goto_instr(int label_num);
instruction_t* create_arithmetic_instr(char* op_name, int reg1, int reg2, int dest_reg);
instruction_t* create_relational_instr(char* op_name, int reg1, int reg2, int dest_reg);
instruction_t* create_loadi_instr(int dest_reg, int value);
instruction_t* create_fun_instr(char* func_name, int num_params);
instruction_t* create_end_instr();
instruction_t* create_ret_instr(int has_return_value, int reg);
instruction_t* create_alloc_instr(char* var_name, char* scope, int size);
instruction_t* create_load_instr(int dest_reg, char* var_name, char* scope, int has_index, int index_reg);
instruction_t* create_store_instr(int src_reg, char* var_name, char* scope, int has_index, int index_reg);
instruction_t* create_arg_instr(int reg, int param_num);
instruction_t* create_iff_instr(int cond_reg, int label_num);


void assembly();
void initialize_assembly();
assembly_t* create_assembly_node(instruction_type_t type, char *name);
void print_assembly();
void free_assembly();
assembly_t* create_r_instruction(char* name, int rd, int rs, int rt);
assembly_t* create_i_instruction(char* name, int rt, int rs, int immediate);
assembly_t* create_j_instruction(char* name, char* label);
void add_assembly_instruction(assembly_t* instruction);


void initialize_labels();
label_t* create_label_node(char* id, int address);
void add_label(char* id, int address);
int get_label_address(char* id);
void free_labels();
void print_labels();


void initialize_memory(memory_t* memory);
function_memory_t* insert_function(memory_t *memory, char *func_name);
void insert_variable(function_memory_t* func, char *var_name, var_type_t type);
variable_t* get_variable(function_memory_t* func, char *var_name);
void print_memory();
function_memory_t* search_function(memory_t* memory, char* func_name);
int get_sp_relation(function_memory_t* func, variable_t* var);
int get_fp_relation(function_memory_t* func, variable_t* var);
int get_sp(function_memory_t* func);
int get_fp(function_memory_t* func);
void delete_temp(function_memory_t* func);
void free_memory_table();


void binary(FILE* file);
void binary_debug(FILE* file);

#endif 
