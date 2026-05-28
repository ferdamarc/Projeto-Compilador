#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <unistd.h>

/* Incluir configurações de layout de memória compartilhadas */
#include "../include/memory_layout.h"

#define PATH_SEPARATOR '/'

#define NOP_OPCODE "00000011111111111111100000100000"

#define DEFAULT_INPUT_DIR "inputs_bins"
#define DEFAULT_SO_BIN_PATH "binSO.txt"

#define DEFAULT_FINAL_DIR "output"
#define DEFAULT_FINAL_NAME "single_port_rom_init.txt"

#define MAX_FILES MAX_PROGS
#define MAX_LINE 256

#define TIER_SIMPLE  0U
#define TIER_COMPLEX 1U

struct file_order_entry {
    const char *name;
    unsigned order;     /* posição dentro do tier (1-based) */
    unsigned tier;      /* TIER_SIMPLE ou TIER_COMPLEX */
};


static const struct file_order_entry g_file_order[] = {
    /* Tier simples — slot de 300 inst, base START_IM (2000) */
    {"area",                1, TIER_SIMPLE},
    {"contagem regressiva", 2, TIER_SIMPLE},
    {"fibonacci",           3, TIER_SIMPLE},
    {"fatorial",            4, TIER_SIMPLE},
    {"gcd",                 5, TIER_SIMPLE},
    {"media",               6, TIER_SIMPLE},
    {"simple",              7, TIER_SIMPLE},
    {"paridade",            8, TIER_SIMPLE},
    {"potencia",            9, TIER_SIMPLE},
    {"soma vetores",       10, TIER_SIMPLE},

    /* Tier complexo — slot único de PROGRAM_INTERVAL_COMPLEX inst, base START_IM_COMPLEX (5000) */
    {"tic tac toe",         1, TIER_COMPLEX}
};

struct file_entry {
    char name[260];
    unsigned order;
    unsigned tier;
};

static int get_order(const char *filename, unsigned *order_out, unsigned *tier_out) {
    static char normalized[260];
    size_t src = 0U;
    size_t dst = 0U;
    /* drop extension */
    while (filename[src] != '\0' && filename[src] != '.') {
        char c = filename[src];
        if (c == '_' || c == '-') {
            normalized[dst++] = ' ';
        } else if (c >= 'A' && c <= 'Z') {
            normalized[dst++] = (char)(c - 'A' + 'a');
        } else {
            normalized[dst++] = c;
        }
        src++;
        if (dst + 1U >= sizeof(normalized)) {
            return 0;
        }
    }
    normalized[dst] = '\0';

    for (size_t i = 0U; i < sizeof(g_file_order) / sizeof(g_file_order[0]); ++i) {
        if (strcmp(normalized, g_file_order[i].name) == 0) {
            *order_out = g_file_order[i].order;
            *tier_out  = g_file_order[i].tier;
            return 1;
        }
    }
    return 0;
}

static int compare_entries(const void *a, const void *b) {
    const struct file_entry *fa = (const struct file_entry *)a;
    const struct file_entry *fb = (const struct file_entry *)b;
    if (fa->tier != fb->tier) {
        return (fa->tier < fb->tier) ? -1 : 1;
    }
    if (fa->order != fb->order) {
        return (fa->order < fb->order) ? -1 : 1;
    }
    return strcmp(fa->name, fb->name);
}

static void trim(char *line) {
    size_t len = strlen(line);
    while (len > 0U) {
        char c = line[len - 1U];
        if (c == '\r' || c == '\n' || c == ' ' || c == '\t') {
            line[len - 1U] = '\0';
            len--;
        } else {
            break;
        }
    }
}

static void build_path(const char *root, const char *filename, char *out, size_t out_size) {
    size_t len = strlen(root);
    if (len > 0U) {
        char last = root[len - 1U];
        if (last == '/' || last == '\\') {
            snprintf(out, out_size, "%s%s", root, filename);
            return;
        }
    }
    snprintf(out, out_size, "%s%c%s", root, PATH_SEPARATOR, filename);
}

static int ensure_directory_exists(const char *path) {
    char buffer[512];
    size_t len = strlen(path);
    if (len == 0U || len >= sizeof(buffer)) {
        errno = ENAMETOOLONG;
        return 0;
    }

    strncpy(buffer, path, sizeof(buffer) - 1U);
    buffer[sizeof(buffer) - 1U] = '\0';

    size_t i = 0U;
    if (buffer[0] == '/' || buffer[0] == '\\') {
        i = 1U;
    }

    for (; i < len; ++i) {
        char c = buffer[i];
        if (c == '/' || c == '\\') {
            char saved = buffer[i];
            buffer[i] = '\0';
            if (buffer[0] != '\0') {
                if (mkdir(buffer, 0777) != 0 && errno != EEXIST) {
                    return 0;
                }
            }
            buffer[i] = saved;
        }
    }

    if (mkdir(buffer, 0777) != 0 && errno != EEXIST) {
        return 0;
    }
    return 1;
}



int main(int argc, char **argv) {
    const char *input_dir = DEFAULT_INPUT_DIR;
    const char *so_path = DEFAULT_SO_BIN_PATH;
    const char *output_dir = DEFAULT_FINAL_DIR;
    const char *output_name = DEFAULT_FINAL_NAME;

    if (argc >= 2 && argv[1][0] != '\0') {
        input_dir = argv[1];
    }
    if (argc >= 3 && argv[2][0] != '\0') {
        so_path = argv[2];
    }
    if (argc >= 4 && argv[3][0] != '\0') {
        output_dir = argv[3];
    }
    if (argc >= 5 && argv[4][0] != '\0') {
        output_name = argv[4];
    }

    char final_path[512];
    build_path(output_dir, output_name, final_path, sizeof(final_path));
    struct file_entry files[MAX_FILES];
    size_t file_count = 0U;

    /* Garante que o diretório de saída (e pais necessários) exista */
    if (!ensure_directory_exists(output_dir)) {
        perror("Nao foi possivel criar diretorio de saida");
        return EXIT_FAILURE;
    }

    DIR *dir = opendir(input_dir);
    if (!dir) {
        perror("Nao foi possivel listar diretorio de entrada");
        return EXIT_FAILURE;
    }

    struct dirent *entry_dir;
    while ((entry_dir = readdir(dir)) != NULL) {
        const char *fname = entry_dir->d_name;
        if (strcmp(fname, ".") == 0 || strcmp(fname, "..") == 0) {
            continue;
        }

        unsigned order = 0U;
        unsigned tier  = 0U;
        if (!get_order(fname, &order, &tier)) {
            continue;
        }

        char candidate_path[512];
        build_path(input_dir, fname, candidate_path, sizeof(candidate_path));
        struct stat st;
        if (stat(candidate_path, &st) != 0 || !S_ISREG(st.st_mode)) {
            continue;
        }

        if (file_count >= MAX_FILES) {
            fprintf(stderr, "Limite de arquivos excedido.\n");
            closedir(dir);
            return EXIT_FAILURE;
        }

        strncpy(files[file_count].name, fname, sizeof(files[file_count].name) - 1U);
        files[file_count].name[sizeof(files[file_count].name) - 1U] = '\0';
        files[file_count].order = order;
        files[file_count].tier  = tier;
        file_count++;
    }
    closedir(dir);

    qsort(files, file_count, sizeof(files[0]), compare_entries);

    FILE *fout = fopen(final_path, "w");
    if (!fout) {
        perror("Nao foi possivel criar arquivo final");
        return EXIT_FAILURE;
    }

    FILE *fso = fopen(so_path, "r");
    if (!fso) {
        perror("Nao foi possivel abrir binario do SO");
        fclose(fout);
        return EXIT_FAILURE;
    }

    char line[MAX_LINE];
    unsigned line_count = 0U;
    while (fgets(line, sizeof(line), fso)) {
        trim(line);
        if (line[0] == '\0') {
            continue;
        }
        fprintf(fout, "%s\n", line);
        line_count++;
    }
    fclose(fso);

    while (line_count < SO_INTERVAL) {
        fprintf(fout, "%s\n", NOP_OPCODE);
        line_count++;
    }

    /* --- Tier simples: MAX_PROGS_SIMPLE slots, cada um com PROGRAM_INTERVAL --- */
    unsigned simple_slots_filled = 0U;
    for (size_t idx = 0U; idx < file_count; ++idx) {
        const struct file_entry *entry = &files[idx];
        if (entry->tier != TIER_SIMPLE) {
            continue;
        }
        char path[512];
        build_path(input_dir, entry->name, path, sizeof(path));
        printf("Concatenando (simples) arquivo: %s...\n", entry->name);

        FILE *fin = fopen(path, "r");
        if (!fin) {
            fprintf(stderr, "Nao foi possivel abrir '%s'.\n", path);
            fclose(fout);
            return EXIT_FAILURE;
        }

        unsigned written = 0U;
        while (fgets(line, sizeof(line), fin)) {
            trim(line);
            if (line[0] == '\0') {
                continue;
            }
            fprintf(fout, "%s\n", line);
            written++;
        }
        fclose(fin);

        if (written > PROGRAM_INTERVAL) {
            fprintf(stderr, "Programa simples '%s' tem %u inst, excede limite %u.\n",
                    entry->name, written, PROGRAM_INTERVAL);
            fclose(fout);
            return EXIT_FAILURE;
        }
        while (written < PROGRAM_INTERVAL) {
            fprintf(fout, "%s\n", NOP_OPCODE);
            written++;
        }
        simple_slots_filled++;
    }
    /* Preenche slots simples vazios com NOPs para manter offsets fixos */
    while (simple_slots_filled < MAX_PROGS_SIMPLE) {
        for (unsigned i = 0U; i < PROGRAM_INTERVAL; ++i) {
            fprintf(fout, "%s\n", NOP_OPCODE);
        }
        simple_slots_filled++;
    }

    /* --- Tier complexo: MAX_PROGS_COMPLEX slots, cada um com PROGRAM_INTERVAL_COMPLEX --- */
    unsigned complex_slots_filled = 0U;
    for (size_t idx = 0U; idx < file_count; ++idx) {
        const struct file_entry *entry = &files[idx];
        if (entry->tier != TIER_COMPLEX) {
            continue;
        }
        if (complex_slots_filled >= MAX_PROGS_COMPLEX) {
            fprintf(stderr, "Mais programas complexos que MAX_PROGS_COMPLEX (%d) — '%s' descartado.\n",
                    MAX_PROGS_COMPLEX, entry->name);
            break;
        }
        char path[512];
        build_path(input_dir, entry->name, path, sizeof(path));
        printf("Concatenando (complexo) arquivo: %s...\n", entry->name);

        FILE *fin = fopen(path, "r");
        if (!fin) {
            fprintf(stderr, "Nao foi possivel abrir '%s'.\n", path);
            fclose(fout);
            return EXIT_FAILURE;
        }

        unsigned written = 0U;
        while (fgets(line, sizeof(line), fin)) {
            trim(line);
            if (line[0] == '\0') {
                continue;
            }
            fprintf(fout, "%s\n", line);
            written++;
        }
        fclose(fin);

        if (written > PROGRAM_INTERVAL_COMPLEX) {
            fprintf(stderr, "Programa complexo '%s' tem %u inst, excede limite %u.\n",
                    entry->name, written, PROGRAM_INTERVAL_COMPLEX);
            fclose(fout);
            return EXIT_FAILURE;
        }
        while (written < PROGRAM_INTERVAL_COMPLEX) {
            fprintf(fout, "%s\n", NOP_OPCODE);
            written++;
        }
        complex_slots_filled++;
    }
    while (complex_slots_filled < MAX_PROGS_COMPLEX) {
        for (unsigned i = 0U; i < PROGRAM_INTERVAL_COMPLEX; ++i) {
            fprintf(fout, "%s\n", NOP_OPCODE);
        }
        complex_slots_filled++;
    }

    fclose(fout);
    printf("Concatenacao finalizada com sucesso!\n");
    return EXIT_SUCCESS;
}
