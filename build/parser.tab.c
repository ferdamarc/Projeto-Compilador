/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser.y"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "analysis.h"

#define YYSTYPE ast_node_ptr
#define MAX_NODES 10000

static int yylex(void);
void yyerror(char* s);
int yyparse(void);
void show_tree(ast_node_ptr root, int num);
enum yytokentype get_token(void);
ast_node_ptr parse(void);

enum yytokentype aux_error;
int syntax_errors = 0;

ast_node_ptr syntax_tree;       // Raiz da árvore sintática
ast_node_ptr nodes[MAX_NODES];  // Array de nós da árvore
int node_count = 0;             // Contador de nós
char aux_lexeme[MAXLEXEMA];     // Buffer auxiliar


#line 97 "./build/parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUM = 3,                        /* NUM  */
  YYSYMBOL_SOMA = 4,                       /* SOMA  */
  YYSYMBOL_SUB = 5,                        /* SUB  */
  YYSYMBOL_MULT = 6,                       /* MULT  */
  YYSYMBOL_DIV = 7,                        /* DIV  */
  YYSYMBOL_INT = 8,                        /* INT  */
  YYSYMBOL_ID = 9,                         /* ID  */
  YYSYMBOL_VOID = 10,                      /* VOID  */
  YYSYMBOL_WHILE = 11,                     /* WHILE  */
  YYSYMBOL_ELSE = 12,                      /* ELSE  */
  YYSYMBOL_IF = 13,                        /* IF  */
  YYSYMBOL_ABREPARENTESES = 14,            /* ABREPARENTESES  */
  YYSYMBOL_FECHAPARENTESES = 15,           /* FECHAPARENTESES  */
  YYSYMBOL_RETURN = 16,                    /* RETURN  */
  YYSYMBOL_COMMA = 17,                     /* COMMA  */
  YYSYMBOL_ABRECHAVES = 18,                /* ABRECHAVES  */
  YYSYMBOL_FECHACHAVES = 19,               /* FECHACHAVES  */
  YYSYMBOL_SEMICOLON = 20,                 /* SEMICOLON  */
  YYSYMBOL_ATRIB = 21,                     /* ATRIB  */
  YYSYMBOL_ABRECOLCHETES = 22,             /* ABRECOLCHETES  */
  YYSYMBOL_FECHACOLCHETES = 23,            /* FECHACOLCHETES  */
  YYSYMBOL_EQ = 24,                        /* EQ  */
  YYSYMBOL_NEQ = 25,                       /* NEQ  */
  YYSYMBOL_LT = 26,                        /* LT  */
  YYSYMBOL_LET = 27,                       /* LET  */
  YYSYMBOL_GT = 28,                        /* GT  */
  YYSYMBOL_GET = 29,                       /* GET  */
  YYSYMBOL_ERRO = 30,                      /* ERRO  */
  YYSYMBOL_IFX = 31,                       /* IFX  */
  YYSYMBOL_YYACCEPT = 32,                  /* $accept  */
  YYSYMBOL_programa = 33,                  /* programa  */
  YYSYMBOL_declaracao_lista = 34,          /* declaracao_lista  */
  YYSYMBOL_declaracao = 35,                /* declaracao  */
  YYSYMBOL_var_declaracao = 36,            /* var_declaracao  */
  YYSYMBOL_tipo_especificador = 37,        /* tipo_especificador  */
  YYSYMBOL_fun_declaracao = 38,            /* fun_declaracao  */
  YYSYMBOL_fun_id = 39,                    /* fun_id  */
  YYSYMBOL_params = 40,                    /* params  */
  YYSYMBOL_param_lista = 41,               /* param_lista  */
  YYSYMBOL_param = 42,                     /* param  */
  YYSYMBOL_composto_decl = 43,             /* composto_decl  */
  YYSYMBOL_local_declaracoes = 44,         /* local_declaracoes  */
  YYSYMBOL_statement_lista = 45,           /* statement_lista  */
  YYSYMBOL_statement = 46,                 /* statement  */
  YYSYMBOL_expressao_decl = 47,            /* expressao_decl  */
  YYSYMBOL_selecao_decl = 48,              /* selecao_decl  */
  YYSYMBOL_iteracao_decl = 49,             /* iteracao_decl  */
  YYSYMBOL_retorno_decl = 50,              /* retorno_decl  */
  YYSYMBOL_expressao = 51,                 /* expressao  */
  YYSYMBOL_var = 52,                       /* var  */
  YYSYMBOL_simples_expressao = 53,         /* simples_expressao  */
  YYSYMBOL_relacional = 54,                /* relacional  */
  YYSYMBOL_operador_relacional = 55,       /* operador_relacional  */
  YYSYMBOL_soma_expressao = 56,            /* soma_expressao  */
  YYSYMBOL_soma = 57,                      /* soma  */
  YYSYMBOL_termo = 58,                     /* termo  */
  YYSYMBOL_mult = 59,                      /* mult  */
  YYSYMBOL_fator = 60,                     /* fator  */
  YYSYMBOL_ativacao = 61,                  /* ativacao  */
  YYSYMBOL_args = 62,                      /* args  */
  YYSYMBOL_arg_lista = 63                  /* arg_lista  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   114

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  111

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   286


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    40,    40,    46,    54,    58,    59,    63,    76,    82,
     100,   109,   115,   124,   135,   144,   145,   155,   163,   166,
     177,   191,   201,   209,   212,   220,   224,   225,   226,   227,
     228,   232,   233,   234,   243,   253,   267,   280,   288,   300,
     310,   314,   322,   334,   342,   345,   349,   350,   351,   352,
     353,   354,   358,   366,   370,   371,   375,   383,   387,   388,
     392,   393,   394,   395,   406,   416,   417,   420,   428
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUM", "SOMA", "SUB",
  "MULT", "DIV", "INT", "ID", "VOID", "WHILE", "ELSE", "IF",
  "ABREPARENTESES", "FECHAPARENTESES", "RETURN", "COMMA", "ABRECHAVES",
  "FECHACHAVES", "SEMICOLON", "ATRIB", "ABRECOLCHETES", "FECHACOLCHETES",
  "EQ", "NEQ", "LT", "LET", "GT", "GET", "ERRO", "IFX", "$accept",
  "programa", "declaracao_lista", "declaracao", "var_declaracao",
  "tipo_especificador", "fun_declaracao", "fun_id", "params",
  "param_lista", "param", "composto_decl", "local_declaracoes",
  "statement_lista", "statement", "expressao_decl", "selecao_decl",
  "iteracao_decl", "retorno_decl", "expressao", "var", "simples_expressao",
  "relacional", "operador_relacional", "soma_expressao", "soma", "termo",
  "mult", "fator", "ativacao", "args", "arg_lista", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-65)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-17)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      60,   -65,   -65,    24,    60,   -65,   -65,     1,   -65,   -65,
     -65,    34,    51,   -11,   -65,   -13,   -65,    52,    64,   -65,
      14,    61,    53,    62,    58,   -65,    59,    56,    63,    60,
     -65,    57,   -65,   -65,   -65,   -65,    60,   -65,    17,    33,
      51,    65,   -65,    21,    68,    69,     2,     3,   -65,   -65,
      70,   -65,   -65,   -65,   -65,   -65,   -65,    66,    67,   -65,
       4,     7,   -65,   -65,   -65,     2,     2,     2,    72,   -65,
      71,     2,   -65,     2,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,     2,   -65,     2,   -65,   -65,     2,    73,    74,
      75,   -65,   -65,   -65,    77,    76,   -65,   -65,    36,     7,
     -65,   -65,    47,    47,   -65,     2,   -65,    82,   -65,    47,
     -65
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    11,    12,     0,     2,     4,     5,     0,     6,     1,
       3,     0,    14,     0,     8,     0,     7,     0,     0,    10,
       0,    12,     0,     0,    15,    18,     0,    19,     0,     0,
       9,     0,    23,    13,    17,    20,    25,    22,     0,     0,
       0,     0,    63,    41,     0,     0,     0,     0,    21,    32,
       0,    27,    24,    26,    28,    29,    30,     0,    61,    40,
      44,    53,    57,    62,    33,     0,     0,     0,     0,    37,
       0,    66,    31,     0,    54,    55,    46,    47,    48,    50,
      49,    51,     0,    45,     0,    58,    59,     0,     0,     0,
       0,    60,    38,    68,     0,    65,    39,    61,    43,    52,
      56,    42,     0,     0,    64,     0,    36,    34,    67,     0,
      35
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,   -65,   -65,    91,    78,   -14,   -65,    90,   -65,   -65,
      79,    81,   -65,   -65,   -64,   -65,   -65,   -65,   -65,   -46,
     -18,   -65,   -65,   -65,    16,   -65,    15,   -65,    13,   -65,
     -65,   -65
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     3,     4,     5,     6,     7,     8,    50,    23,    24,
      25,    51,    36,    39,    52,    53,    54,    55,    56,    57,
      58,    59,    82,    83,    60,    84,    61,    87,    62,    63,
      94,    95
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      68,    70,    11,    18,    22,    42,    42,    19,    74,    75,
      12,    43,    43,    85,    86,    22,    46,    46,    11,    88,
      89,    90,    38,    69,     9,    93,    40,    96,    76,    77,
      78,    79,    80,    81,    41,   -14,    42,    26,   106,   107,
      74,    75,    43,    65,    44,   110,    45,    46,    41,    47,
      42,    32,    48,    49,    14,    20,    43,    15,    44,   108,
      45,    46,    27,    47,    97,    32,    97,    49,     1,    97,
       2,    16,     1,    17,    21,    29,   -16,    28,    31,    30,
      35,    32,    66,    67,    71,    64,    72,    91,    73,   102,
     103,    92,   104,   105,   109,    10,   101,    13,    98,    99,
     100,     0,     0,     0,     0,     0,     0,     0,    34,    33,
       0,     0,     0,     0,    37
};

static const yytype_int8 yycheck[] =
{
      46,    47,     1,    14,    18,     3,     3,    20,     4,     5,
       9,     9,     9,     6,     7,    29,    14,    14,     1,    65,
      66,    67,    36,    20,     0,    71,     9,    73,    24,    25,
      26,    27,    28,    29,     1,    14,     3,    23,   102,   103,
       4,     5,     9,    22,    11,   109,    13,    14,     1,    16,
       3,    18,    19,    20,    20,     3,     9,    23,    11,   105,
      13,    14,     9,    16,    82,    18,    84,    20,     8,    87,
      10,    20,     8,    22,    10,    17,    15,    15,    22,    20,
      23,    18,    14,    14,    14,    20,    20,    15,    21,    15,
      15,    20,    15,    17,    12,     4,    23,     7,    82,    84,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    28,
      -1,    -1,    -1,    -1,    36
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     8,    10,    33,    34,    35,    36,    37,    38,     0,
      35,     1,     9,    39,    20,    23,    20,    22,    14,    20,
       3,    10,    37,    40,    41,    42,    23,     9,    15,    17,
      20,    22,    18,    43,    42,    23,    44,    36,    37,    45,
       9,     1,     3,     9,    11,    13,    14,    16,    19,    20,
      39,    43,    46,    47,    48,    49,    50,    51,    52,    53,
      56,    58,    60,    61,    20,    22,    14,    14,    51,    20,
      51,    14,    20,    21,     4,     5,    24,    25,    26,    27,
      28,    29,    54,    55,    57,     6,     7,    59,    51,    51,
      51,    15,    20,    51,    62,    63,    51,    52,    56,    58,
      60,    23,    15,    15,    15,    17,    46,    46,    51,    12,
      46
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    32,    33,    34,    34,    35,    35,    36,    36,    36,
      36,    37,    37,    38,    39,    40,    40,    41,    41,    42,
      42,    43,    44,    44,    45,    45,    46,    46,    46,    46,
      46,    47,    47,    47,    48,    48,    49,    50,    50,    51,
      51,    52,    52,    53,    53,    54,    55,    55,    55,    55,
      55,    55,    56,    56,    57,    57,    58,    58,    59,    59,
      60,    60,    60,    60,    61,    62,    62,    63,    63
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     3,     3,     6,
       4,     1,     1,     6,     1,     1,     1,     3,     1,     2,
       4,     4,     2,     0,     2,     0,     1,     1,     1,     1,
       1,     2,     1,     2,     5,     7,     5,     2,     3,     3,
       1,     1,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       3,     1,     1,     1,     4,     1,     0,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* programa: declaracao_lista  */
#line 40 "src/parser.y"
                                                   {
                        syntax_tree = yyvsp[0];
                    }
#line 1227 "./build/parser.tab.c"
    break;

  case 3: /* declaracao_lista: declaracao_lista declaracao  */
#line 46 "src/parser.y"
                                                      {
                        if (yyvsp[-1] != NULL) {
                            yyval = yyvsp[-1];
                            add_sibling(yyval, yyvsp[0]);
                        } else {
                            yyval = yyvsp[0];
                        }
                    }
#line 1240 "./build/parser.tab.c"
    break;

  case 4: /* declaracao_lista: declaracao  */
#line 54 "src/parser.y"
                                 { yyval = yyvsp[0]; }
#line 1246 "./build/parser.tab.c"
    break;

  case 5: /* declaracao: var_declaracao  */
#line 58 "src/parser.y"
                                                 { yyval = yyvsp[0]; }
#line 1252 "./build/parser.tab.c"
    break;

  case 6: /* declaracao: fun_declaracao  */
#line 59 "src/parser.y"
                                     { yyval = yyvsp[0]; }
#line 1258 "./build/parser.tab.c"
    break;

  case 7: /* var_declaracao: tipo_especificador ID SEMICOLON  */
#line 63 "src/parser.y"
                                                          {
                        yyval = yyvsp[-2];
                        yyval->node_type = NODE_DECLARATION;
                        yyval->decl_type = DECL_VAR;
                        yyval->line_num = line_num;
                    
                        ast_node_ptr aux = new_node();
                        strcpy(aux->lexeme, lexeme_stack[stack_index]);
                        stack_index--;
                        add_child(yyval, aux);

                        nodes[node_count++] = aux;
                    }
#line 1276 "./build/parser.tab.c"
    break;

  case 8: /* var_declaracao: tipo_especificador error SEMICOLON  */
#line 76 "src/parser.y"
                                                         {
                        yyerrok;
                        printf(ANSI_COLOR_GREEN "RECUPERAÇÃO DE ERRO: " ANSI_COLOR_RESET);
                        printf("Ignorando declaração inválida\n");
                        yyval = NULL;
                    }
#line 1287 "./build/parser.tab.c"
    break;

  case 9: /* var_declaracao: tipo_especificador ID ABRECOLCHETES NUM FECHACOLCHETES SEMICOLON  */
#line 82 "src/parser.y"
                                                                                       {
                        yyval = yyvsp[-5];
                        yyval->node_type = NODE_DECLARATION;
                        yyval->decl_type = DECL_ARRAY;
                        yyval->line_num = line_num;

                        ast_node_ptr aux = new_node();
                        ast_node_ptr aux2 = new_node();
                        
                        strcpy(aux->lexeme, lexeme_stack[stack_index--]);
                        strcpy(aux2->lexeme, lexeme_stack[stack_index--]);
                        
                        add_child(yyval, aux2);
                        add_child(yyval, aux);

                        nodes[node_count++] = aux;
                        nodes[node_count++] = aux2;
                    }
#line 1310 "./build/parser.tab.c"
    break;

  case 10: /* var_declaracao: tipo_especificador error FECHACOLCHETES SEMICOLON  */
#line 100 "src/parser.y"
                                                                        {
                        yyerrok;
                        printf(ANSI_COLOR_GREEN "RECUPERAÇÃO DE ERRO: " ANSI_COLOR_RESET);
                        printf("Ignorando declaração inválida\n");
                        yyval = NULL;
                    }
#line 1321 "./build/parser.tab.c"
    break;

  case 11: /* tipo_especificador: INT  */
#line 109 "src/parser.y"
                              {
                        yyval = new_node();
                        strcpy(yyval->lexeme, "INT");
                        yyval->line_num = line_num;
                        nodes[node_count++] = yyval;
                    }
#line 1332 "./build/parser.tab.c"
    break;

  case 12: /* tipo_especificador: VOID  */
#line 115 "src/parser.y"
                           {
                        yyval = new_node();
                        strcpy(yyval->lexeme, "VOID");
                        yyval->line_num = line_num;
                        nodes[node_count++] = yyval;
                    }
#line 1343 "./build/parser.tab.c"
    break;

  case 13: /* fun_declaracao: tipo_especificador fun_id ABREPARENTESES params FECHAPARENTESES composto_decl  */
#line 124 "src/parser.y"
                                                                                                        {
                        yyval = yyvsp[-5];
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[-4]);
                        add_child(yyvsp[-4], yyvsp[0]);
                        yyval->node_type = NODE_DECLARATION;
                        yyval->decl_type = DECL_FUNC;
                    }
#line 1356 "./build/parser.tab.c"
    break;

  case 14: /* fun_id: ID  */
#line 135 "src/parser.y"
                                     {
                        yyval = new_node();
                        strcpy(yyval->lexeme, lexeme_stack[stack_index--]);
                        yyval->line_num = line_num;
                        nodes[node_count++] = yyval;
                    }
#line 1367 "./build/parser.tab.c"
    break;

  case 15: /* params: param_lista  */
#line 144 "src/parser.y"
                                              { yyval = yyvsp[0]; }
#line 1373 "./build/parser.tab.c"
    break;

  case 16: /* params: VOID  */
#line 145 "src/parser.y"
                           {
                        yyval = new_node();
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_PARAM_VOID;
                        strcpy(yyval->lexeme, "VOID");
                        nodes[node_count++] = yyval;
                    }
#line 1386 "./build/parser.tab.c"
    break;

  case 17: /* param_lista: param_lista COMMA param  */
#line 155 "src/parser.y"
                                                          {
                        if (yyvsp[-2] != NULL) {
                            yyval = yyvsp[-2];
                            add_sibling(yyval, yyvsp[0]);
                        } else {
                            yyval = yyvsp[0];
                        }
                    }
#line 1399 "./build/parser.tab.c"
    break;

  case 18: /* param_lista: param  */
#line 163 "src/parser.y"
                            { yyval = yyvsp[0]; }
#line 1405 "./build/parser.tab.c"
    break;

  case 19: /* param: tipo_especificador ID  */
#line 166 "src/parser.y"
                                                        {
                        yyval = yyvsp[-1];
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_PARAM_VAR;

                        ast_node_ptr aux = new_node();
                        strcpy(aux->lexeme, lexeme_stack[stack_index--]);
                        add_child(yyval, aux);
                        nodes[node_count++] = aux;
                    }
#line 1421 "./build/parser.tab.c"
    break;

  case 20: /* param: tipo_especificador ID ABRECOLCHETES FECHACOLCHETES  */
#line 177 "src/parser.y"
                                                                         {
                        yyval = yyvsp[-3];
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_PARAM_ARRAY;

                        ast_node_ptr aux = new_node();
                        strcpy(aux->lexeme, lexeme_stack[stack_index--]);
                        add_child(yyval, aux);
                        nodes[node_count++] = aux;
                    }
#line 1437 "./build/parser.tab.c"
    break;

  case 21: /* composto_decl: ABRECHAVES local_declaracoes statement_lista FECHACHAVES  */
#line 191 "src/parser.y"
                                                                                   {
                        if (yyvsp[-2] != NULL) {
                            yyval = yyvsp[-2];
                            add_sibling(yyval, yyvsp[-1]);
                        } else {
                            yyval = yyvsp[-1];
                        }
                    }
#line 1450 "./build/parser.tab.c"
    break;

  case 22: /* local_declaracoes: local_declaracoes var_declaracao  */
#line 201 "src/parser.y"
                                                           {
                        if (yyvsp[-1] != NULL) {
                            yyval = yyvsp[-1];
                            add_sibling(yyval, yyvsp[0]);
                        } else {
                            yyval = yyvsp[0];
                        }
                    }
#line 1463 "./build/parser.tab.c"
    break;

  case 23: /* local_declaracoes: %empty  */
#line 209 "src/parser.y"
                             { yyval = NULL; }
#line 1469 "./build/parser.tab.c"
    break;

  case 24: /* statement_lista: statement_lista statement  */
#line 212 "src/parser.y"
                                                    {
                        if (yyvsp[-1] != NULL) {
                            yyval = yyvsp[-1];
                            add_sibling(yyval, yyvsp[0]);
                        } else {
                            yyval = yyvsp[0];
                        }
                    }
#line 1482 "./build/parser.tab.c"
    break;

  case 25: /* statement_lista: %empty  */
#line 220 "src/parser.y"
                             { yyval = NULL; }
#line 1488 "./build/parser.tab.c"
    break;

  case 26: /* statement: expressao_decl  */
#line 224 "src/parser.y"
                                                 { yyval = yyvsp[0]; }
#line 1494 "./build/parser.tab.c"
    break;

  case 27: /* statement: composto_decl  */
#line 225 "src/parser.y"
                                    { yyval = yyvsp[0]; }
#line 1500 "./build/parser.tab.c"
    break;

  case 28: /* statement: selecao_decl  */
#line 226 "src/parser.y"
                                   { yyval = yyvsp[0]; }
#line 1506 "./build/parser.tab.c"
    break;

  case 29: /* statement: iteracao_decl  */
#line 227 "src/parser.y"
                                    { yyval = yyvsp[0]; }
#line 1512 "./build/parser.tab.c"
    break;

  case 30: /* statement: retorno_decl  */
#line 228 "src/parser.y"
                                   { yyval = yyvsp[0]; }
#line 1518 "./build/parser.tab.c"
    break;

  case 31: /* expressao_decl: expressao SEMICOLON  */
#line 232 "src/parser.y"
                                              { yyval = yyvsp[-1]; }
#line 1524 "./build/parser.tab.c"
    break;

  case 32: /* expressao_decl: SEMICOLON  */
#line 233 "src/parser.y"
                                { yyval = NULL; }
#line 1530 "./build/parser.tab.c"
    break;

  case 33: /* expressao_decl: error SEMICOLON  */
#line 234 "src/parser.y"
                                      {
                        yyerrok;
                        printf(ANSI_COLOR_GREEN "RECUPERAÇÃO DE ERRO: " ANSI_COLOR_RESET);
                        printf("Sincronizando em ';'\n");
                        yyval = NULL;
                    }
#line 1541 "./build/parser.tab.c"
    break;

  case 34: /* selecao_decl: IF ABREPARENTESES expressao FECHAPARENTESES statement  */
#line 243 "src/parser.y"
                                                                                          {
                        yyval = new_node();
                        strcpy(yyval->lexeme, "IF");
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_IF;
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[0]);
                        nodes[node_count++] = yyval;
                    }
#line 1556 "./build/parser.tab.c"
    break;

  case 35: /* selecao_decl: IF ABREPARENTESES expressao FECHAPARENTESES statement ELSE statement  */
#line 253 "src/parser.y"
                                                                                           {
                        yyval = new_node();
                        strcpy(yyval->lexeme, "IF");
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_IF;
                        add_child(yyval, yyvsp[-4]);
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[0]);
                        nodes[node_count++] = yyval;
                    }
#line 1572 "./build/parser.tab.c"
    break;

  case 36: /* iteracao_decl: WHILE ABREPARENTESES expressao FECHAPARENTESES statement  */
#line 267 "src/parser.y"
                                                                                   {
                        yyval = new_node();
                        strcpy(yyval->lexeme, "WHILE");
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_WHILE;
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[0]);
                        nodes[node_count++] = yyval;
                    }
#line 1587 "./build/parser.tab.c"
    break;

  case 37: /* retorno_decl: RETURN SEMICOLON  */
#line 280 "src/parser.y"
                                           {
                        yyval = new_node();
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_RETURN_VOID;
                        strcpy(yyval->lexeme, "ReturnVOID");
                        nodes[node_count++] = yyval;
                    }
#line 1600 "./build/parser.tab.c"
    break;

  case 38: /* retorno_decl: RETURN expressao SEMICOLON  */
#line 288 "src/parser.y"
                                                 {
                        yyval = new_node();
                        yyval->node_type = NODE_DECLARATION;
                        yyval->line_num = line_num;
                        yyval->decl_type = DECL_RETURN_INT;
                        strcpy(yyval->lexeme, "ReturnINT");
                        add_child(yyval, yyvsp[-1]);
                        nodes[node_count++] = yyval;
                    }
#line 1614 "./build/parser.tab.c"
    break;

  case 39: /* expressao: var ATRIB expressao  */
#line 300 "src/parser.y"
                                                      {
                        yyval = new_node();
                        strcpy(yyval->lexeme, "=");
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_ASSIGN;
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[0]);
                        nodes[node_count++] = yyval;
                    }
#line 1629 "./build/parser.tab.c"
    break;

  case 40: /* expressao: simples_expressao  */
#line 310 "src/parser.y"
                                        { yyval = yyvsp[0]; }
#line 1635 "./build/parser.tab.c"
    break;

  case 41: /* var: ID  */
#line 314 "src/parser.y"
                                     {
                        yyval = new_node();
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_ID;
                        strcpy(yyval->lexeme, lexeme_stack[stack_index--]);
                        nodes[node_count++] = yyval;
                    }
#line 1648 "./build/parser.tab.c"
    break;

  case 42: /* var: ID ABRECOLCHETES expressao FECHACOLCHETES  */
#line 322 "src/parser.y"
                                                                {
                        yyval = new_node();
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_ARRAY;
                        strcpy(yyval->lexeme, lexeme_stack[stack_index--]);
                        add_child(yyval, yyvsp[-1]);
                        nodes[node_count++] = yyval;
                    }
#line 1662 "./build/parser.tab.c"
    break;

  case 43: /* simples_expressao: soma_expressao relacional soma_expressao  */
#line 334 "src/parser.y"
                                                                   {
                        yyval = yyvsp[-1];
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_OP_REL;
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[0]);
                    }
#line 1675 "./build/parser.tab.c"
    break;

  case 44: /* simples_expressao: soma_expressao  */
#line 342 "src/parser.y"
                                     { yyval = yyvsp[0]; }
#line 1681 "./build/parser.tab.c"
    break;

  case 45: /* relacional: operador_relacional  */
#line 345 "src/parser.y"
                                                      { yyval = yyvsp[0]; }
#line 1687 "./build/parser.tab.c"
    break;

  case 46: /* operador_relacional: EQ  */
#line 349 "src/parser.y"
                              { yyval = new_node(); strcpy(yyval->lexeme, "=="); nodes[node_count++] = yyval; }
#line 1693 "./build/parser.tab.c"
    break;

  case 47: /* operador_relacional: NEQ  */
#line 350 "src/parser.y"
                          { yyval = new_node(); strcpy(yyval->lexeme, "!="); nodes[node_count++] = yyval; }
#line 1699 "./build/parser.tab.c"
    break;

  case 48: /* operador_relacional: LT  */
#line 351 "src/parser.y"
                          { yyval = new_node(); strcpy(yyval->lexeme, "<");  nodes[node_count++] = yyval; }
#line 1705 "./build/parser.tab.c"
    break;

  case 49: /* operador_relacional: GT  */
#line 352 "src/parser.y"
                          { yyval = new_node(); strcpy(yyval->lexeme, ">");  nodes[node_count++] = yyval; }
#line 1711 "./build/parser.tab.c"
    break;

  case 50: /* operador_relacional: LET  */
#line 353 "src/parser.y"
                          { yyval = new_node(); strcpy(yyval->lexeme, "<="); nodes[node_count++] = yyval; }
#line 1717 "./build/parser.tab.c"
    break;

  case 51: /* operador_relacional: GET  */
#line 354 "src/parser.y"
                          { yyval = new_node(); strcpy(yyval->lexeme, ">="); nodes[node_count++] = yyval; }
#line 1723 "./build/parser.tab.c"
    break;

  case 52: /* soma_expressao: soma_expressao soma termo  */
#line 358 "src/parser.y"
                                                    {
                        yyval = yyvsp[-1];
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_OP;
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[0]);
                    }
#line 1736 "./build/parser.tab.c"
    break;

  case 53: /* soma_expressao: termo  */
#line 366 "src/parser.y"
                            { yyval = yyvsp[0]; }
#line 1742 "./build/parser.tab.c"
    break;

  case 54: /* soma: SOMA  */
#line 370 "src/parser.y"
                                       { yyval = new_node(); strcpy(yyval->lexeme, "+"); nodes[node_count++] = yyval; }
#line 1748 "./build/parser.tab.c"
    break;

  case 55: /* soma: SUB  */
#line 371 "src/parser.y"
                           { yyval = new_node(); strcpy(yyval->lexeme, "-"); nodes[node_count++] = yyval; }
#line 1754 "./build/parser.tab.c"
    break;

  case 56: /* termo: termo mult fator  */
#line 375 "src/parser.y"
                                                   {
                        yyval = yyvsp[-1];
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_OP;
                        add_child(yyval, yyvsp[-2]);
                        add_child(yyval, yyvsp[0]);
                    }
#line 1767 "./build/parser.tab.c"
    break;

  case 57: /* termo: fator  */
#line 383 "src/parser.y"
                            { yyval = yyvsp[0]; }
#line 1773 "./build/parser.tab.c"
    break;

  case 58: /* mult: MULT  */
#line 387 "src/parser.y"
                                       { yyval = new_node(); strcpy(yyval->lexeme, "*"); nodes[node_count++] = yyval; }
#line 1779 "./build/parser.tab.c"
    break;

  case 59: /* mult: DIV  */
#line 388 "src/parser.y"
                           { yyval = new_node(); strcpy(yyval->lexeme, "/"); nodes[node_count++] = yyval; }
#line 1785 "./build/parser.tab.c"
    break;

  case 60: /* fator: ABREPARENTESES expressao FECHAPARENTESES  */
#line 392 "src/parser.y"
                                                                           { yyval = yyvsp[-1]; }
#line 1791 "./build/parser.tab.c"
    break;

  case 61: /* fator: var  */
#line 393 "src/parser.y"
                          { yyval = yyvsp[0]; }
#line 1797 "./build/parser.tab.c"
    break;

  case 62: /* fator: ativacao  */
#line 394 "src/parser.y"
                               { yyval = yyvsp[0]; }
#line 1803 "./build/parser.tab.c"
    break;

  case 63: /* fator: NUM  */
#line 395 "src/parser.y"
                          {
                        yyval = new_node();
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_CONST;
                        strcpy(yyval->lexeme, lexeme_stack[stack_index--]);
                        nodes[node_count++] = yyval;
                    }
#line 1816 "./build/parser.tab.c"
    break;

  case 64: /* ativacao: fun_id ABREPARENTESES args FECHAPARENTESES  */
#line 406 "src/parser.y"
                                                                             {
                        yyval = yyvsp[-3];
                        yyval->node_type = NODE_EXPRESSION;
                        yyval->line_num = line_num;
                        yyval->expr_type = EXPR_CALL;
                        add_child(yyval, yyvsp[-1]);
                    }
#line 1828 "./build/parser.tab.c"
    break;

  case 65: /* args: arg_lista  */
#line 416 "src/parser.y"
                                            { yyval = yyvsp[0]; }
#line 1834 "./build/parser.tab.c"
    break;

  case 66: /* args: %empty  */
#line 417 "src/parser.y"
                             { yyval = NULL; }
#line 1840 "./build/parser.tab.c"
    break;

  case 67: /* arg_lista: arg_lista COMMA expressao  */
#line 420 "src/parser.y"
                                                            {
                        if (yyvsp[-2] != NULL) {
                            yyval = yyvsp[-2];
                            add_sibling(yyval, yyvsp[0]);
                        } else {
                            yyval = yyvsp[0];
                        }
                    }
#line 1853 "./build/parser.tab.c"
    break;

  case 68: /* arg_lista: expressao  */
#line 428 "src/parser.y"
                                { yyval = yyvsp[0]; }
#line 1859 "./build/parser.tab.c"
    break;


#line 1863 "./build/parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 431 "src/parser.y"


/* Tratamento de erros sintáticos */
/* Tentativa simples de tentar mapear vários erros da análise sintática;
 * A utilização do yyerrok faz-se possível para conseguir mapear os erros.
*/
void yyerror(char *s) {
    printf(ANSI_COLOR_YELLOW "ERRO SINTÁTICO: " ANSI_COLOR_RESET);
    printf(ANSI_COLOR_WHITE "\"%s\" ", yytext);
    printf(ANSI_COLOR_YELLOW "LINHA: " ANSI_COLOR_WHITE "%d" ANSI_COLOR_RESET, line_num);
    printf(" | %s\n", s);
    syntax_errors++;
}

// Única interface com o analisador léxico
int yylex(void) {
    return (aux_error = get_token());
}

// Função principal de parsing
ast_node_ptr parse(void) {
    yyparse();
    return syntax_tree;
}

