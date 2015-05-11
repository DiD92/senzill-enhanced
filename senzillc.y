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
int current_type;
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
void install ( char *sym_name, int sym_type ) 
{ 
  symrec *s = getsym (sym_name); 
  if (s == 0) 
    s = putsym (sym_name, sym_type); 
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
Generate specific READ instruction for type
-------------------------------------------------------------------------*/
int gen_read_code( int sym_type )
{
  char message[ 100 ];
  switch(sym_type) {
    case(T_INTEGER):
      return READ_INT;
    case(T_REAL):
      return READ_REAL;
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
  switch(sym_type) {
    case(T_INTEGER):
      return LD_VAR_I;
    case(T_REAL):
      return LD_VAR_R;
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
   char *id; /* Identifiers */ 
   struct lbs *lbls; /* For backpatching */ 
};

/*========================================================================= 
TOKENS 
=========================================================================*/ 
%start program 
%token <intval> NUMBER /* Simple integer */
%token <rval> RNUMBER /* Simple float */
%token <id> IDENTIFIER /* Simple identifier */ 
%token <lbls> IF WHILE /* For backpatching labels */ 
%token SKIP THEN ELSE FI DO END 
%token INTEGER REAL READ OUT LET IN 
%token ASSGNOP 

/*========================================================================= 
OPERATOR PRECEDENCE 
=========================================================================*/ 
%left '-' '+' 
%left '*' '/' 
%right '^' 

/*========================================================================= 
GRAMMAR RULES for the Simple language 
=========================================================================*/ 

%% 

program : LET declarations IN { gen_code( DATA, gen_stack_elem_i( data_location() - 1 ) ); } 
          commands END { gen_code( HALT, gen_stack_elem_i( 0 ) ); YYACCEPT; } 
;

declarations : /* empty */
    | declarations declaration '.'

declaration : /* empty */ 
    | INTEGER { current_type = T_INTEGER; } id_seq IDENTIFIER { install( $4, T_INTEGER ); }
    | REAL { current_type = T_REAL; } id_seq IDENTIFIER { install( $4, T_REAL ); }
; 

id_seq : /* empty */ 
    | id_seq IDENTIFIER ',' { install( $2, current_type ); } 
; 

commands : /* empty */ 
    | commands command ';' 
; 

command : SKIP 
   | READ IDENTIFIER { gen_code( gen_read_code( get_sym_type( $2 ) ), gen_stack_elem_i( context_check( $2 ) ) ); } 
   | OUT exp { gen_code( WRITE, gen_stack_elem_i( 0 ) ); } 
   | IDENTIFIER ASSGNOP exp { gen_code( STORE, gen_stack_elem_i( context_check( $1 ) ) ); } 
   | IF bool_exp { $1 = (struct lbs *) newlblrec(); $1->for_jmp_false = reserve_loc(); } 
   THEN commands { $1->for_goto = reserve_loc(); } ELSE { 
     back_patch( $1->for_jmp_false, JMP_FALSE, gen_stack_elem_i( gen_label() ) ); 
   } commands FI { back_patch( $1->for_goto, GOTO, gen_stack_elem_i( gen_label() ) ); } 
   | WHILE { $1 = (struct lbs *) newlblrec(); $1->for_goto = gen_label(); } 
   bool_exp { $1->for_jmp_false = reserve_loc(); } DO commands END { gen_code( GOTO, gen_stack_elem_i( $1->for_goto ) ); 
   back_patch( $1->for_jmp_false, JMP_FALSE, gen_stack_elem_i( gen_label() ) ); } 
;

bool_exp : exp '<' exp { gen_code( LT, gen_stack_elem_i( 0 ) ); } 
   | exp '=' exp { gen_code( EQ, gen_stack_elem_i( 0 ) ); } 
   | exp '>' exp { gen_code( GT, gen_stack_elem_i( 0 ) ); } 
;

exp : NUMBER { gen_code( LD_INT, gen_stack_elem_i( $1 ) ); }
   | RNUMBER { gen_code( LD_REAL, gen_stack_elem_r( $1 ) ); }
   | IDENTIFIER { gen_code( gen_load_code( get_sym_type( $1 ) ), gen_stack_elem_i( context_check( $1 ) ) ); } 
   | exp '+' exp { gen_code( ADD, gen_stack_elem_i( 0 ) ); } 
   | exp '-' exp { gen_code( SUB, gen_stack_elem_i( 0 ) ); } 
   | exp '*' exp { gen_code( MULT, gen_stack_elem_i( 0 ) ); } 
   | exp '/' exp { gen_code( DIV, gen_stack_elem_i( 0 ) ); } 
   | exp '^' exp { gen_code( PWR, gen_stack_elem_i( 0 ) ); } 
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

