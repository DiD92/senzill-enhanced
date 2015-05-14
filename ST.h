/*------------------------------------------------------------------------- 
SYMBOL TABLE RECORD 
-------------------------------------------------------------------------*/
#define MAX_DEPTH 20

enum var_types { T_INTEGER, T_REAL, T_STRING, T_ERR };

struct symrec 
{ 
  char *name; /* name of symbol */
  int type; /* type of symbol */
  int offset; /* data offset */ 
  struct symrec *next; /* link field */ 
}; 
typedef struct symrec symrec; 

extern symrec sym_stack[MAX_DEPTH];

symrec * getsym (char *sym_name);
symrec * putsym (char *sym_name, int sym_type);

