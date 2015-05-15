/*************************************************************************** 
  Code Generator
***************************************************************************/ 

#ifndef __CG_H
#define __CG_H

#include "SM.h"

struct file_code
{
	int code_op;
	stack_elem arg;
};
typedef struct file_code file_code;

extern int code_offset;

int data_location();
void gen_code( enum code_ops operation, stack_elem *arg );
int reserve_loc();
void back_patch( int addr, enum code_ops operation, stack_elem *arg );
int gen_label();
void print_code();
void read_bytecode( char *file_name );
file_code read_type( FILE *file);
void write_bytecode( char *file_name );
int write_type( FILE *file, file_code bytecode );

#endif
