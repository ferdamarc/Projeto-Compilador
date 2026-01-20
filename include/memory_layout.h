#ifndef MEMORY_LAYOUT_H
#define MEMORY_LAYOUT_H

/* Intervalo reservado para o Sistema Operacional na ROM */
#define SO_INTERVAL 1000

/* Endereço inicial dos programas de usuário na ROM */
#define START_IM 1000

/* Espaço alocado para cada programa de usuário (em instruções) */
#define PROGRAM_INTERVAL 300

/* Número máximo de programas de usuário */
#define MAX_PROGS 10


/* Espaço reservado para dados do SO (0..499) */
#define SO_DATA_RESERVED 500

/* Endereço inicial da área de dados do primeiro programa */
#define START_DM 500

/* Espaço alocado para dados de cada programa (em palavras) */
#define PROG_INTERVAL_DM 500


/*
 * Calcula o endereço base de instruções para o programa N (1-based)
 * Fórmula: START_IM + (N-1) * PROG_INTERVAL
 */
#define PROG_IM_BASE(N) (START_IM + (N - 1) * PROGRAM_INTERVAL)

/*
 * Calcula o endereço base de dados para o programa N (1-based)
 * Fórmula: START_DM + (N-1) * PROG_INTERVAL_DM
 */
#define PROG_DM_BASE(N) (START_DM + (N - 1) * PROG_INTERVAL_DM)

#endif
