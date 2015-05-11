/*------------------------------------------------------------------------- 
SYMBOL TABLE RECORD 
-------------------------------------------------------------------------*/
enum var_types { T_INTEGER, T_REAL, T_ERR };

struct symrec 
{ 
  char *name; /* name of symbol */
  int type; /* type of symbol */
  int offset; /* data offset */ 
  struct symrec *next; /* link field */ 
}; 
typedef struct symrec symrec; 

symrec * getsym (char *sym_name);
symrec * putsym (char *sym_name, int sym_type);

