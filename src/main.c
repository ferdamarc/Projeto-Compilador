#include "compiler.h"
#include "parser.h"

// Estrutura de configuração do compilador
typedef struct {
    int debug;
    char* inputFilename;
    char* outputFilename;
} CompilerConfig;

// Variáveis globais
int flag_verbose = 0;
FILE *input_file = NULL;
FILE *copy_file = NULL;
FILE *output_file = NULL;
FILE *output_intermediate_file = NULL;
FILE *output_assembly_file = NULL;

// Protótipos
static void printUsage(const char* programName);
void free_analysis_structures(ast_node_ptr syntax_tree, symbol_item_ptr* hash_table);
void free_synthesis_structures();


int main(int argc, char *argv[]) {
    CompilerConfig config = {0, NULL, NULL};
    FILE *binary_output_file = NULL;
    line_num = 1;

    // Verificação de argumentos
    if (argc < 2) {
        printUsage(argv[0]);
        return 0;
    }

    // Parsing de argumentos da linha de comando
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-D") == 0) {
            config.debug = 1;
        }
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-O") == 0) {
            if (i + 1 < argc) {
                config.outputFilename = argv[++i];
            } else {
                printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
                printf("Opcao -o requer um nome de arquivo.\n");
                return 1;
            }
        }
        else {
            if (config.inputFilename == NULL) {
                config.inputFilename = argv[i];
            } else {
                printf(ANSI_COLOR_RED "Aviso: " ANSI_COLOR_RESET);
                printf("Argumento '%s' desconsiderado.\n", argv[i]);
            }
        }
    }

    // Verifica se arquivo de entrada foi especificado
    if (config.inputFilename == NULL) {
        printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
        printf("Nenhum arquivo de entrada especificado.\n");
        printUsage(argv[0]);
        return 1;
    }

    // Abertura de arquivos
    {
        input_file = fopen(config.inputFilename, "r");
        if (input_file == NULL) {
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("Arquivo '%s' nao encontrado.\n", config.inputFilename);
            return 1;
        }
    }

    if (config.debug) {
        output_file = fopen("output/saida_analise.txt", "w");
        if (output_file == NULL) {
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("Nao foi possivel criar arquivo de resultados.\n");
            output_file = stdout;
        }
    } else {
        output_file = stdout;
    }

    if (config.debug) {
        output_intermediate_file = fopen("output/codigo_intermediario.txt", "w");
        if (output_intermediate_file == NULL) {
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("Nao foi possivel criar arquivo de codigo intermediario.\n");
            output_intermediate_file = stdout;
        }
    } else {
        output_intermediate_file = stdout;
    }

    if (config.debug) {
        output_assembly_file = fopen("output/codigo_assembly.txt", "w");
        if (output_assembly_file == NULL) {
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("Nao foi possivel criar arquivo de codigo assembly.\n");
            output_assembly_file = stdout;
        }
    } else {
        output_assembly_file = stdout;
    }

    if (config.outputFilename != NULL) {
        binary_output_file = fopen(config.outputFilename, "w");
        if (binary_output_file == NULL) {
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("Nao foi possivel criar arquivo de saida '%s'.\n", config.outputFilename);
            fclose(input_file);
            if (config.debug && output_file != stdout) fclose(output_file);
            return 1;
        }
    }

    flag_verbose = config.debug;

    printf(ANSI_COLOR_GREEN "Compilando...\n" ANSI_COLOR_RESET);

    // Fase de análise
    ast_node_ptr syntax_tree = parse();

    fclose(input_file);
    if (copy_file != NULL) fclose(copy_file);
    remove("copia.txt");

    // Inicializa tabela de símbolos com funções built-in
    symbol_item_ptr* hash_table = initialize_symbol_table();
    insert_symbol(hash_table, DECL_FUNC, TYPE_INT,  "input", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "output", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "show_lcd", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "no_op", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "os_jump_to", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_INT,  "get_pc", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "os_set_dm_base", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "os_set_im_base", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "os_save_return", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "os_save_context", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "os_restore_context", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "set_interr_timer", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_INT,  "get_interr_type", "global", 0);
    insert_symbol(hash_table, DECL_FUNC, TYPE_VOID, "os_set_pc", "global", 0);

    // Análise semântica
    if (syntax_tree != NULL) {
        traverse_tree(syntax_tree, hash_table, "global");
        
        if (search_symbol_expr(hash_table, "main", "global", EXPR_CALL) == NULL) {
            show_semantic_error(FUNC_MAIN_NOT_DECLARED, "main", line_num);
        }
    } else {
        printf(ANSI_COLOR_RED "AVISO: " ANSI_COLOR_RESET);
        printf("Arvore sintatica nao foi criada devido a erros lexicos/sintaticos.\n");
        printf("Pulando analise semantica detalhada.\n");
    }

    // Output da análise
    if (config.debug) {
        show_tree(syntax_tree, 0);
        fprintf(output_file, "\n\n");
        print_symbol_table(hash_table);
        fclose(output_file);
    } else {
        remove("output/saida_analise.txt");
    }

    // Verifica erros antes de continuar
    int totalErros = lexical_errors + syntax_errors + semantic_errors;
    if (totalErros > 0) {
        printf(ANSI_COLOR_RED);
        printf("\n========== RESUMO DA ANALISE ==========\n");
        if (lexical_errors > 0) {
            printf("Erros lexicos encontrados: %d\n", lexical_errors);
        }
        if (syntax_errors > 0) {
            printf("Erros sintaticos encontrados: %d\n", syntax_errors);
        }
        if (semantic_errors > 0) {
            printf("Erros semanticos encontrados: %d\n", semantic_errors);
        }
        printf("Total de erros: %d\n", totalErros);
        printf("==========================================\n");
        printf("Nao foi possivel gerar o codigo devido aos erros encontrados.\n");
        printf(ANSI_COLOR_RESET);

        free_analysis_structures(syntax_tree, hash_table);
        
        if (config.debug && output_intermediate_file != stdout) 
            fclose(output_intermediate_file);
        if (config.debug && output_assembly_file != stdout) 
            fclose(output_assembly_file);
        if (binary_output_file != NULL) 
            fclose(binary_output_file);

        return 0;
    }

    // Fase de síntese
    
    // Geração de CI
    initialize_vector();
    create_intermediate_code(syntax_tree, hash_table, 1);

    intermediate_code[array_index++] = create_instruction("HALT");

    if (config.debug) {
        print_intermediate_code();
        fclose(output_intermediate_file);
        //show_reg();
    } else {
        remove("output/codigo_intermediario.txt");
    }

    free_analysis_structures(syntax_tree, hash_table);

    // Geração de código assembly
    assembly();

    if (config.debug) {
        print_assembly();
        fclose(output_assembly_file);
        print_memory();
        print_labels();
    } else {
        remove("output/codigo_assembly.txt");
    }

    // Geração de código binário
    if (binary_output_file == NULL) {
        binary_output_file = fopen("output/binario_final.txt", "w");
        if (binary_output_file == NULL) {
            printf(ANSI_COLOR_RED "Erro: " ANSI_COLOR_RESET);
            printf("Nao foi possivel criar arquivo de saida binario.\n");
            free_synthesis_structures();
            return 1;
        }
    }

    binary(binary_output_file);

    fclose(binary_output_file);
    
    if (config.debug) {
        FILE* arquivo_debug = fopen("output/debug.txt", "w");
        if (arquivo_debug != NULL) {
            binary_debug(arquivo_debug);
            fclose(arquivo_debug);
        }
    }

    free_synthesis_structures();

    printf(ANSI_COLOR_GREEN "Compilacao realizada com sucesso!\n" ANSI_COLOR_RESET);

    return 0;
}

void free_synthesis_structures() {
    deallocate_vector();
    free_assembly();
    free_memory_table();
    free_labels();
}

void free_analysis_structures(ast_node_ptr syntax_tree, symbol_item_ptr* hash_table) {
    free_tree(syntax_tree);
    delete_symbol_table(hash_table);
}

static void printUsage(const char* programName) {
    printf("\nUso: %s <arquivo.cm> [opcoes]\n\n", programName);
    printf("Opcoes:\n");
    printf("  -d, -D              Modo debug (gera todos os arquivos de analise e sintese)\n");
    printf("  -o, -O <arquivo>    Especifica arquivo de saida binario\n\n");
    printf("Exemplos:\n");
    printf("  %s programa.cm                    (gera apenas binario)\n", programName);
    printf("  %s programa.cm -d                 (modo debug - gera todos os arquivos)\n", programName);
    printf("  %s programa.cm -o saida.bin       (binario customizado)\n", programName);
    printf("  %s programa.cm -d -o saida.bin    (debug + binario customizado)\n\n", programName);
}