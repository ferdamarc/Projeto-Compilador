#ifndef MEMORY_LAYOUT_H
#define MEMORY_LAYOUT_H

/* ===== Instrução (MI) =====
 * Layout total (INSTR_ADDR_WIDTH = 13 → 8192 posições):
 *
 *   0 .... 1999    SO                          (2000 inst)
 *   2000 . 4999    PCs simples (10 × 300)      (300 cada)
 *   5000 . 8191    PC complexo único           (3192 inst)
 */

/* SO */
#define SO_INTERVAL              2000

/* Tier simples — IDs 1..10 */
#define START_IM                 2000
#define PROGRAM_INTERVAL         300
#define MAX_PROGS_SIMPLE         10

/* Tier complexo — ID 11 (único) */
#define START_IM_COMPLEX         (START_IM + PROGRAM_INTERVAL * MAX_PROGS_SIMPLE)  /* 5000 */
#define PROGRAM_INTERVAL_COMPLEX 3192       /* 8192 - 5000 */
#define MAX_PROGS_COMPLEX        1
#define COMPLEX_PROG_ID          (MAX_PROGS_SIMPLE + 1)                            /* 11 */

#define MAX_PROGS                (MAX_PROGS_SIMPLE + MAX_PROGS_COMPLEX)            /* 11 */


/* ===== Dados (MD) =====
 * Mesmo padrão da MI (DATA_ADDR_WIDTH = 13 → 8192 palavras):
 *
 *   0 ..... 499    SO                          (500 palavras)
 *   500 .. 5499    PCs simples (10 × 500)      (500 cada)
 *   5500 . 8191    PC complexo único           (2692 palavras)
 */

#define SO_DATA_RESERVED         500
#define START_DM                 500
#define PROG_INTERVAL_DM         500       /* tier simples */

#define START_DM_COMPLEX         (START_DM + PROG_INTERVAL_DM * MAX_PROGS_SIMPLE)  /* 5500 */
#define PROG_INTERVAL_DM_COMPLEX 2692      /* 8192 - 5500 */


/* ===== Helpers =====
 * Endereço base de instruções/dados para o programa N (1-based).
 * Simples: IDs 1..10. Complexo: ID 11.
 */
#define PROG_IM_BASE(N) \
    ((N) <= MAX_PROGS_SIMPLE                                                       \
        ? (START_IM         + ((N) - 1)               * PROGRAM_INTERVAL)          \
        : (START_IM_COMPLEX + ((N) - COMPLEX_PROG_ID) * PROGRAM_INTERVAL_COMPLEX))

#define PROG_DM_BASE(N) \
    ((N) <= MAX_PROGS_SIMPLE                                                       \
        ? (START_DM         + ((N) - 1)               * PROG_INTERVAL_DM)          \
        : (START_DM_COMPLEX + ((N) - COMPLEX_PROG_ID) * PROG_INTERVAL_DM_COMPLEX))

#endif
