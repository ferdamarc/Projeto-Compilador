#include "synthesis.h"

#define MAX_REG 22 // Numero maximo de registradores

char temp_string[MAXLEXEMA] = "Temporario"; // Nome para identificar variaveis temporarias

/* Essa variavel sera importante para o discarte dos registradores, ja
que registradores usados pela ultima vez a algum tempo serao eliminados primeiro
do que os usados recentemente, para tentar evitar conflitos em instrucoes */
int total_reg = 1;

int total_reg_in_use = 0; 

typedef struct reg{
    int num_reg;
    char* var_name;
    char scope[MAXLEXEMA];
    int discard; // Diz se o registrador pode ser descartado (1, 2, ..., n) ou nao (0)
    int last_use; // Timestamp do ultimo uso para algoritmo LRU
}cpu_register_t;

cpu_register_t register_list[MAX_REG]; // Lista encadeada com os registradores

// Funcao para inicializar o vetor de registradores
void initialize_registers(){
    for(int i = 0; i < MAX_REG; i++){
        register_list[i].num_reg = i;
        register_list[i].var_name = NULL;
        strcpy(register_list[i].scope, "");
        register_list[i].discard = 0;
        register_list[i].last_use = 0;
    }
}

// Adiciona uma variavel em um registrador
int add_var_register(char* var_name, char* scope){
    for(int i = 0; i < MAX_REG; i++){
        if(register_list[i].var_name == NULL){
            register_list[i].var_name = var_name;
            strcpy(register_list[i].scope, scope);
            register_list[i].discard = 0;
            register_list[i].last_use = total_reg;
            total_reg++;
            total_reg_in_use++;
            return i;
        }
    }
    return -1; // Nao foi possivel adicionar a variavel em nenhum registrador
}

// Adiciona uma variavel temporaria em um registrador, normalmente utilizada em operacoes
int add_temp_register(){
    for(int i = 0; i < MAX_REG; i++){
        if(register_list[i].var_name == NULL){
            register_list[i].var_name = temp_string;
            strcpy(register_list[i].scope, func_name);
            register_list[i].discard = total_reg;
            register_list[i].last_use = total_reg;
            total_reg++;
            total_reg_in_use++;
            return i;
        }
    }
    return -1; // Nao foi possivel adicionar a variavel em nenhum registrador
}

int search_var_register(char* var_name, char* scope){
    for(int i = 0; i < MAX_REG; i++){
        if(register_list[i].var_name != NULL){
            if(strcmp(register_list[i].var_name, var_name) == 0 && strcmp(register_list[i].scope, scope) == 0){
                register_list[i].last_use = total_reg; // Atualiza ultimo uso para LRU
                total_reg++;
                return i;
            }
        }
    }

    return -1; // Nao foi possivel encontrar a variavel em nenhum registrador
}

// Funcao para mostrar os registradores e suas informacoes na tela do usuario
void show_registers(){
    int cont = 0;
    printf("\n============== Registradores ===============\n");
    for(int i = 0; i < MAX_REG; i++){
        if(register_list[i].var_name != NULL){
            printf("t%d: %s, %s, %d, uso:%d\n", register_list[i].num_reg, register_list[i].var_name, register_list[i].scope, register_list[i].discard, register_list[i].last_use);
        }
    }
    printf(ANSI_COLOR_PURPLE);
    printf("%d Registradores Livres\n\n", MAX_REG - total_reg_in_use);
    printf(ANSI_COLOR_RESET);
}

// Funcao para descartar um registrador usando algoritmo LRU (Least Recently Used)
int discard_register(){
    int lowest_last_use = total_reg + 1;
    int discarded_reg = -1;

    // Primeiro: tenta descartar entre registradores temporarios (temp_string)
    for(int i = 0; i < MAX_REG; i++){	
        if(register_list[i].var_name != NULL && strcmp(register_list[i].var_name, temp_string) == 0){ 
            if(register_list[i].last_use < lowest_last_use){
                lowest_last_use = register_list[i].last_use;
                discarded_reg = i;
            }
        }
    }

    // Segundo: se nao encontrou temporarios, descarta qualquer registrador
    if(discarded_reg == -1){
        for(int i = 0; i < MAX_REG; i++){	
            if(register_list[i].var_name != NULL){ 
                if(register_list[i].last_use < lowest_last_use){
                    lowest_last_use = register_list[i].last_use;
                    discarded_reg = i;
                }
            }
        }
    }

    // Se ainda nao encontrou, erro
    if(discarded_reg == -1){
        fprintf(stderr, ANSI_COLOR_RED "ERRO: Nao foi possivel descartar nenhum registrador\n" ANSI_COLOR_RESET);
        return -1;
    }

    // Limpa o registrador descartado
    register_list[discarded_reg].var_name = NULL;
    strcpy(register_list[discarded_reg].scope, "");
    register_list[discarded_reg].discard = 0;
    register_list[discarded_reg].last_use = 0;
    total_reg_in_use--;
    

    //if(DEBUG_MODE){
    //    if(register_list[discarded_reg].var_name == temp_string) {
    //        fprintf(stderr, ANSI_COLOR_PURPLE "LRU DESCARTE: " ANSI_COLOR_RESET "Registrador t%d (TEMPORARIO, last_use:%d) descartado\n", 
    //                discarded_reg, lowest_last_use);
    //    } else {
    //        fprintf(stderr, ANSI_COLOR_PURPLE "LRU DESCARTE: " ANSI_COLOR_RESET "Registrador t%d (VARIAVEL '%s', last_use:%d) descartado\n", 
    //                discarded_reg, register_list[discarded_reg].var_name ? register_list[discarded_reg].var_name : "NULL", lowest_last_use);
    //    }
    //}
    
    return discarded_reg;
}

/* Funcao para verificar se a variavel ja esta em um registrador
Se nao estiver, adiciona a variavel em um registrador
Se estiver, retorna o numero do registrador em que a variavel esta
Se nao for possivel adicionar a variavel em um registrador, retorna -1 */
int check_registers(char *lexeme, char* scope, int is_temp){
    int reg;

    if(is_temp == 0){
        if((reg = (search_var_register(lexeme, scope))) == -1){
            if(total_reg_in_use == MAX_REG){
                if((reg = discard_register()) == -1){
                    printf(ANSI_COLOR_RED);
                    printf("ERRO: Nao foi possivel descartar nenhum registrador\n");
                    printf(ANSI_COLOR_RESET);
                    return -1;
                }
            }
            
            if((reg = add_var_register(lexeme, scope)) == -1){
                printf(ANSI_COLOR_RED);
                printf("Erro ao adicionar variavel no vetor de registradores");
                printf(ANSI_COLOR_RESET);
            }
        }

        return reg;
    }

    if(total_reg_in_use == MAX_REG){
        if((reg = discard_register()) == -1){
            printf(ANSI_COLOR_RED);
            printf("ERRO: Nao foi possivel descartar nenhum registrador\n");
            printf(ANSI_COLOR_RESET);
            return -1;
        }
    }
    if((reg = add_temp_register()) == -1){
        printf(ANSI_COLOR_RED);
        printf("Erro ao adicionar variavel no vetor de registradores");
        printf(ANSI_COLOR_RESET);
    }

    return reg;
    

}