/*************************************************************************** 
Code Generator 
Author: Anthony A. Aaby
Modified by: Jordi Planes
***************************************************************************/ 

#include <stdio.h>
#include "ST.h"
#include "SM.h"
#include "CG.h"

/*------------------------------------------------------------------------- 
Data Segment 
-------------------------------------------------------------------------*/ 
int data_offset = 0; /* Initial offset */ 
int data_location() /* Reserves a data location */ 
{ 
  return data_offset++; 
} 

/*------------------------------------------------------------------------- 
  Code Segment 
  -------------------------------------------------------------------------*/ 
int code_offset = 0; /* Initial offset */ 
int gen_label() /* Returns current offset */ 
{ 
  return code_offset; 
} 

int reserve_loc() /* Reserves a code location */ 
{ 
  return code_offset++; 
} 

/* Generates code at current location */ 
void gen_code( enum code_ops operation, stack_elem *arg ) 
{ 
  code[code_offset].op = operation; 
  code[code_offset++].arg = *arg; 
} 

/* Generates code at a reserved location */ 
void back_patch( int addr, enum code_ops operation, stack_elem *arg ) 
{ 
  code[addr].op = operation; 
  code[addr].arg = *arg; 
} 

/*------------------------------------------------------------------------- 
Print Code to stdio 
-------------------------------------------------------------------------*/ 
void print_code() 
{ 
  int i = 0;
  while (i < code_offset) {
    switch(code[i].arg.v_type) {
      case T_REAL:
        printf("%3d: %-10s%4.4f\n",i,op_name[(int) code[i].op], code[i].arg.rv );
        break;
      case T_INTEGER:
        printf("%3d: %-10s%4d\n",i,op_name[(int) code[i].op], code[i].arg.iv );
        break;
      default:
        break;
    }
    i++; 
  } 
} 

/* Reads code from file */

void read_bytecode( char *file_name ) {
  FILE * bytecode_file = fopen( file_name, "r" );
  file_code bytecode;
  int i;  

  fread( &code_offset, sizeof( code_offset ), 1, bytecode_file  );
  fread( &data_offset, sizeof( data_offset ), 1, bytecode_file  );
#ifndef NDEBUG
  printf("Offsets: %d %d\n", code_offset, data_offset );
#endif
  for( i = 0; i < MAX_MEMORY && fread( &bytecode, sizeof( file_code ), 1, bytecode_file  ) != 0; i++ ) {
    back_patch( i, bytecode.code_op, &bytecode.arg );
  }
  fclose( bytecode_file );
}

/* Writes code to file */

void write_bytecode( char *file_name ) {
  FILE * bytecode_file = fopen( file_name, "w" );
  file_code bytecode;
  int i;
  
  fwrite( &code_offset, sizeof( code_offset ), 1, bytecode_file  );
  fwrite( &data_offset, sizeof( data_offset ), 1, bytecode_file  );
  for( i = 0; i < MAX_MEMORY; i++ ) {
    bytecode.code_op = code[ i ].op;
    bytecode.arg = code[ i ].arg;
    fwrite( &bytecode, sizeof( file_code ), 1, bytecode_file  );
  }
  fclose( bytecode_file );
}

/************************** End Code Generator **************************/ 
