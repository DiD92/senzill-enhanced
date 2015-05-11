/*************************************************************************** 
  Stack Machine
***************************************************************************/ 

#ifndef __SM_H
#define __SM_H
/*========================================================================= 
DECLARATIONS 
=========================================================================*/ 
/* OPERATIONS: Internal Representation */ 
enum code_ops { HALT, STORE, JMP_FALSE, GOTO, CALL, RET,
		DATA, LD_INT, LD_REAL, LD_VAR_I, LD_VAR_R, 
		READ_INT, READ_REAL, WRITE, 
		LT, EQ, GT, ADD, SUB, MULT, DIV, PWR };

struct stack_elem
{
	int v_type;
	union 
	{
		int iv;
		float rv;
	};
};
typedef struct stack_elem stack_elem;

struct instruction 
{ 
  enum code_ops op;
  stack_elem arg;
}; 

#define MAX_MEMORY 999

void fetch_execute_cycle();
stack_elem * gen_stack_elem_i(int);
stack_elem * gen_stack_elem_r(float);

extern struct instruction code[ MAX_MEMORY ];
extern char *op_name[];

#endif
