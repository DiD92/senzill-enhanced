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
		DATA, LD_INT, LD_REAL, LD_STR, LD_VAR_I, LD_VAR_R,
		LD_VAR_S, READ_INT, READ_REAL, READ_STR, WRITE, 
		LT, EQ, GT, LE, GE, NE, AND, OR, NOT, ADD, SUB, MULT, 
		DIV, PWR, NEG, SLEN };

struct stack_elem
{
	int v_type; // type identifier
	union 		// type container
	{
		int iv; // integers and stack addresses
		float rv; // float values
		struct { // string values
			int len;
			char *str;
		};
	};
};
typedef struct stack_elem stack_elem;

struct instruction 
{ 
  enum code_ops op;
  stack_elem arg;
};

#define MAX_CHARS 256

#define MAX_MEMORY 999

void fetch_execute_cycle();
stack_elem * gen_stack_elem_i(int);
stack_elem * gen_stack_elem_r(float);
stack_elem * gen_stack_elem_s(char*);

extern struct instruction code[ MAX_MEMORY ];
extern char *op_name[];

#endif
