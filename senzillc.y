%{
/************************************************************************* 
Compiler for the Simple language 
Author: Anthony A. Aaby
Modified by: Jordi Planes
***************************************************************************/ 
/*========================================================================= 
C Libraries, Symbol Table, Code Generator & other C code 
=========================================================================*/ 
#include <stdio.h> /* For I/O */ 
#include <stdlib.h> /* For malloc here and in symbol table */ 
#include <string.h> /* For strcmp in symbol table */ 
#include "ST.h" /* Symbol Table */ 
#include "SM.h" /* Stack Machine */ 
#include "CG.h" /* Code Generator */ 

#define YYDEBUG 1 /* For Debugging */ 

int yyerror(char *);
int yylex();

int errors; /* Error Count */
/*------------------------------------------------------------------------- 
Support variables
-------------------------------------------------------------------------*/ 
int c_t;
int v_c;
/*------------------------------------------------------------------------- 
The following support backpatching 
-------------------------------------------------------------------------*/ 
struct lbs /* Labels for data, if and while */ 
{ 
  int for_goto; 
  int for_jmp_false; 
}; 

struct lbs * newlblrec() /* Allocate space for the labels */ 
{ 
   return (struct lbs *) malloc(sizeof(struct lbs)); 
}

/*------------------------------------------------------------------------- 
Install identifier & check if previously defined. 
-------------------------------------------------------------------------*/ 
void install ( char *sym_name, int sym_type, int sym_const ) 
{ 
  symrec *s = getsym (sym_name); 
  if (s == 0) 
    s = putsym (sym_name, sym_type, sym_const); 
  else { 
    char message[ 100 ];
    sprintf( message, "%s is already defined\n", sym_name ); 
    yyerror( message );
  } 
} 

/*------------------------------------------------------------------------- 
If identifier is defined, generate code 
-------------------------------------------------------------------------*/ 
int context_check( char *sym_name ) 
{ 
  symrec *identifier = getsym( sym_name );
  if(identifier == NULL) {
    char message[ 100 ];
    sprintf( message, "%s is not defined\n", sym_name ); 
    yyerror( message );
    return -1;
  } else {
    return identifier->offset;
  }
}

/*------------------------------------------------------------------------- 
Get type for identifier if defined
-------------------------------------------------------------------------*/
int get_sym_type( char *sym_name )
{
  symrec *identifier = getsym( sym_name );
  if(identifier == NULL) {
    char message[ 100 ];
    sprintf( message, "%s is not defined\n", sym_name ); 
    yyerror( message );
    return T_ERR;
  } else {
    return identifier->type;
  }
}

/*------------------------------------------------------------------------- 
Get type for identifier if defined
-------------------------------------------------------------------------*/
int get_sym_const( char *sym_name )
{
  symrec *identifier = getsym( sym_name );
  if(identifier == NULL) {
    char message[ 100 ];
    sprintf( message, "%s is not defined\n", sym_name ); 
    yyerror( message );
    return V_ERR;
  } else {
    return identifier->cnst;
  }
}

/*------------------------------------------------------------------------- 
Generate specific READ instruction for type
-------------------------------------------------------------------------*/
int gen_read_code( int sym_type )
{
  char message[ 100 ];
  switch (sym_type) {
    case T_INTEGER:
      return READ_INT;
    case T_REAL:
      return READ_REAL;
    case T_STRING:
      return READ_STR;
    default:
      sprintf( message, "symbol type not recognized\n" ); 
      yyerror( message );
      return HALT;
  }
}

/*------------------------------------------------------------------------- 
Generate specific LD_VAR instruction for type
-------------------------------------------------------------------------*/
int gen_load_code( int sym_type )
{
  char message[ 100 ];
  switch (sym_type) {
    case T_INTEGER:
      return LD_VAR_I;
    case T_REAL:
      return LD_VAR_R;
    case T_STRING:
      return LD_VAR_S;
    default:
      sprintf( message, "symbol type not recognized\n" ); 
      yyerror( message );
      return HALT;
  }
}

/*========================================================================= 
SEMANTIC RECORDS 
=========================================================================*/ 
%} 

%union semrec /* The Semantic Records */ 
 { 
   int intval; /* Integer values */
   float rval; /* Real values */
   struct { /* String values */
     int len;
     char *str;
   } strval;
   char *id; /* Identifiers */ 
   struct lbs *lbls; /* For backpatching */ 
};

/*========================================================================= 
TOKENS 
=========================================================================*/ 
%start program 
%token <intval> NUMBER /* Simple integer */
%token <rval> RNUMBER /* Simple float */
%token <strval> CHARS /* Simple string */
%token <id> IDENTIFIER /* Simple identifier */ 
%token <lbls> IF WHILE /* For backpatching labels */ 
%token SKIP THEN ELSE FI DO END 
%token INTEGER REAL STRING READ OUT LET IN LENGTH
%token ASSGNOP GRTOEQ LWROEQ NOTEQ CONST

/*========================================================================= 
OPERATOR PRECEDENCE 
=========================================================================*/ 
%left '-' '+' 
%left '*' '/'
%left ANDOP OROP
%right '!'
%right '^' 

/*========================================================================= 
GRAMMAR RULES for the Simple language 
=========================================================================*/ 

%% 

program : LET declarations IN { gen_code( DATA, gen_stack_elem_i( data_location() - 1 ) ); } 
          commands END { gen_code( HALT, gen_stack_elem_i( 0 ) ); YYACCEPT; } 
;

declarations : /* empty */
    | declarations declaration ';'

declaration : /* empty */ 
    | INTEGER { c_t = T_INTEGER; v_c = V_NCONST; } 
      id_seq IDENTIFIER { install( $4, T_INTEGER, V_NCONST ); }
    | REAL { c_t = T_REAL; v_c = V_NCONST; } 
      id_seq IDENTIFIER { install( $4, T_REAL, V_NCONST ); }
    | STRING { c_t = T_STRING; v_c = V_NCONST; } 
      id_seq IDENTIFIER { install( $4, T_STRING, V_NCONST ); }
    | CONST %prec '!' INTEGER { c_t = T_INTEGER; v_c = V_CONST; } 
      cintid_seq IDENTIFIER ASSGNOP NUMBER { install( $5, T_INTEGER, V_CONST ); 
      gen_code( LD_INT, gen_stack_elem_i( $7 ) ); 
      gen_code( STORE, gen_stack_elem_i( data_offset - 1 ) ); }
    | CONST %prec '!' REAL { c_t = T_REAL; v_c = V_CONST; } 
      crealid_seq IDENTIFIER ASSGNOP RNUMBER { install( $5, T_REAL, V_CONST ); 
      gen_code( LD_REAL, gen_stack_elem_r( $7 ) ); 
      gen_code( STORE, gen_stack_elem_i( data_offset - 1 ) ); }
    | CONST %prec '!' STRING { c_t = T_STRING; v_c = V_CONST; } 
      cstrid_seq IDENTIFIER ASSGNOP CHARS { install( $5, T_STRING, V_CONST ); 
      gen_code( LD_STR, gen_stack_elem_s( $7.str ) ); 
      gen_code( STORE, gen_stack_elem_i( data_offset - 1 ) ); }
; 

id_seq : /* empty */ 
    | id_seq IDENTIFIER ',' { install( $2, c_t, v_c ); }
;

cintid_seq : /* empty */
    | cintid_seq IDENTIFIER ASSGNOP NUMBER ',' { install( $2, T_INTEGER, V_CONST ); 
      gen_code( LD_INT, gen_stack_elem_i( $4 ) ); 
      gen_code( STORE, gen_stack_elem_i( data_offset - 1 ) ); }
;

crealid_seq : /* empty */
    | crealid_seq IDENTIFIER ASSGNOP RNUMBER ',' { install( $2, T_REAL, V_CONST ); 
      gen_code( LD_REAL, gen_stack_elem_r( $4 ) ); 
      gen_code( STORE, gen_stack_elem_i( data_offset - 1 ) ); }
;

cstrid_seq : /* empty */
    | cstrid_seq IDENTIFIER ASSGNOP CHARS ',' { install( $2, T_STRING, V_CONST ); 
      gen_code( LD_STR, gen_stack_elem_s( $4.str ) ); 
      gen_code( STORE, gen_stack_elem_i( data_offset - 1 ) ); }
;

commands : /* empty */ 
    | commands command ';' 
; 

command : SKIP 
   | READ IDENTIFIER { gen_code( gen_read_code( get_sym_type( $2 ) ), 
     gen_stack_elem_i( context_check( $2 ) ) ); } 
   | OUT exp { gen_code( WRITE, gen_stack_elem_i( 0 ) ); }
   | OUT str_exp { gen_code( WRITE, gen_stack_elem_i( 0 ) ); }
   | IDENTIFIER ASSGNOP exp { if( get_sym_const( $1 ) == V_CONST ) { 
     printf("Error symbol %s is constant\n", $1); yyerror("semantic error"); } else { 
     gen_code( STORE, gen_stack_elem_i( context_check( $1 ) ) ); } }
   | IDENTIFIER ASSGNOP str_exp { if( get_sym_const( $1 ) == V_CONST ) { 
     printf("Error symbol %s is constant\n", $1); yyerror("semantic error"); } else { 
     gen_code( STORE, gen_stack_elem_i( context_check( $1 ) ) ); } }
   | IF bool_exp { $1 = (struct lbs *) newlblrec(); $1->for_jmp_false = reserve_loc(); } 
   THEN commands { $1->for_goto = reserve_loc(); } ELSE { 
     back_patch( $1->for_jmp_false, JMP_FALSE, gen_stack_elem_i( gen_label() ) ); 
   } commands FI { back_patch( $1->for_goto, GOTO, gen_stack_elem_i( gen_label() ) ); } 
   | WHILE { $1 = (struct lbs *) newlblrec(); $1->for_goto = gen_label(); } 
     bool_exp { $1->for_jmp_false = reserve_loc(); } DO commands END { 
     gen_code( GOTO, gen_stack_elem_i( $1->for_goto ) ); 
     back_patch( $1->for_jmp_false, JMP_FALSE, gen_stack_elem_i( gen_label() ) ); } 
;

str_exp : CHARS { gen_code( LD_STR, gen_stack_elem_s( $1.str ) ); }
   | str_exp '+' str_exp { gen_code( ADD, gen_stack_elem_i( 0 ) ); }
   | str_exp '*' exp { gen_code( MULT, gen_stack_elem_i( 0 ) ); }
   | exp '*' str_exp { gen_code( MULT, gen_stack_elem_i( 0 ) ); }
   | '(' str_exp ')' { }
;

bool_exp : exp '<' exp { gen_code( LT, gen_stack_elem_i( 0 ) ); } 
   | exp '=' exp { gen_code( EQ, gen_stack_elem_i( 0 ) ); } 
   | exp '>' exp { gen_code( GT, gen_stack_elem_i( 0 ) ); }
   | exp LWROEQ exp { gen_code( LE, gen_stack_elem_i( 0 ) ); }
   | exp GRTOEQ exp { gen_code( GE, gen_stack_elem_i( 0 ) ); }
   | exp NOTEQ exp { gen_code( NE, gen_stack_elem_i( 0 ) ); }
   | bool_exp ANDOP bool_exp { gen_code( AND, gen_stack_elem_i( 0 ) ); }
   | bool_exp OROP bool_exp { gen_code( OR, gen_stack_elem_i( 0 ) ); }
   | '!' bool_exp { gen_code( NOT, gen_stack_elem_i( 0 ) ); }
   | '(' bool_exp ')' { }
;

exp : NUMBER { gen_code( LD_INT, gen_stack_elem_i( $1 ) ); }
   | RNUMBER { gen_code( LD_REAL, gen_stack_elem_r( $1 ) ); }
   | IDENTIFIER { gen_code( gen_load_code( get_sym_type( $1 ) ), 
     gen_stack_elem_i( context_check( $1 ) ) ); } 
   | exp '+' exp { gen_code( ADD, gen_stack_elem_i( 0 ) ); } 
   | exp '-' exp { gen_code( SUB, gen_stack_elem_i( 0 ) ); } 
   | exp '*' exp { gen_code( MULT, gen_stack_elem_i( 0 ) ); } 
   | exp '/' exp { gen_code( DIV, gen_stack_elem_i( 0 ) ); } 
   | exp '^' exp { gen_code( PWR, gen_stack_elem_i( 0 ) ); }
   | '-' %prec '!' exp { gen_code( NEG, gen_stack_elem_i( 0 ) ); }
   | LENGTH %prec '!' str_exp { gen_code( SLEN, gen_stack_elem_i( 0 ) ); }
   | LENGTH %prec '!' exp { gen_code( SLEN, gen_stack_elem_i( 0 ) ); }
   | '(' exp ')' { }
;

%% 

extern struct instruction code[ MAX_MEMORY ];

/*========================================================================= 
MAIN 
=========================================================================*/ 
int main( int argc, char *argv[] ) 
{ 
  extern FILE *yyin; 
  if ( argc < 3 ) {
    printf("usage <input-file> <output-file>\n");
    return -1;
  }
  yyin = fopen( argv[1], "r" ); 
  /*yydebug = 1;*/ 
  errors = 0; 
  printf("Senzill Compiler\n");
  yyparse (); 
  printf ( "Parse Completed\n" ); 
  if ( errors == 0 ) 
    { 
      //print_code (); 
      //fetch_execute_cycle();
      write_bytecode( argv[2] );
    } 
  return 0;
} 

/*========================================================================= 
YYERROR 
=========================================================================*/ 
int yyerror ( char *s ) /* Called by yyparse on error */ 
{ 
  errors++; 
  printf ("%s\n", s); 
  return 0;
} 
/**************************** End Grammar File ***************************/ 

