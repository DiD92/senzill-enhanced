/*************************************************************************** 
Code Generator 
Author: Anthony A. Aaby
Modified by: Jordi Planes
***************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
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
        printf("%3d: %-10s%4.4f\n",i,op_name[(int) code[i].op], code[i].arg.rv);
        break;
      case T_INTEGER:
        printf("%3d: %-10s%4d\n",i,op_name[(int) code[i].op], code[i].arg.iv);
        break;
      case T_STRING:
        printf("%3d: %-10s%s\n",i,op_name[(int) code[i].op], code[i].arg.str);
        break;
      default:
        printf("%3d: UNKNOWN COMMAND\n", i);
        return;
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
  for( i = 0; ( i < code_offset ) && 
       ( ( bytecode = read_type( bytecode_file ) ).arg.v_type != T_ERR );
       i++ ) {
    back_patch( i, bytecode.code_op, &bytecode.arg );
  }
  fclose( bytecode_file );
}

/* Reads type values from file */

file_code read_type( FILE *file ) {
  file_code type;
  char *tmp_buff;
  fread( &type.code_op, sizeof( int ), 1, file );
  fread( &type.arg.v_type, sizeof( int ), 1, file );
  switch (type.arg.v_type) {
    case T_INTEGER:
      fread( &type.arg.iv, sizeof( int ), 1, file) ;
      break;
    case T_REAL:
      fread( &type.arg.rv, sizeof( float ), 1, file );
      break;
    case T_STRING:
      fread( &type.arg.len, sizeof( int ), 1, file );
      tmp_buff = (char*) malloc( sizeof( char ) * type.arg.len );
      fread( tmp_buff, sizeof( char ), type.arg.len, file );
      type.arg.str = tmp_buff;
      break;
    default:
      type.arg.v_type = T_ERR;
  }
  return type;
}

/* Writes code to file */

void write_bytecode( char *file_name ) {
  FILE * bytecode_file = fopen( file_name, "w" );
  file_code bytecode;
  int i;

  fwrite( &code_offset, sizeof( code_offset ), 1, bytecode_file  );
  fwrite( &data_offset, sizeof( data_offset ), 1, bytecode_file  );
  for( i = 0; i < code_offset; i++ ) {
    bytecode.code_op = code[ i ].op;
    bytecode.arg = code[ i ].arg;
    if( write_type( bytecode_file, bytecode ) ) {
      printf("Error writing type\n");
      return;
    }  
  }
  fclose( bytecode_file );
}

/* Writes type values to file */

int write_type( FILE *file, file_code bytecode ) {
  char *tmp_buff;
  fwrite( &bytecode.code_op, sizeof( int ), 1, file );
  fwrite( &bytecode.arg.v_type, sizeof( int ), 1, file );
  switch (bytecode.arg.v_type) {
    case T_INTEGER:
      fwrite( &bytecode.arg.iv, sizeof( int ), 1, file );
      return 0;
    case T_REAL:
      fwrite( &bytecode.arg.rv, sizeof( float ), 1, file );
      return 0;
    case T_STRING:
      tmp_buff = bytecode.arg.str;
      fwrite( &bytecode.arg.len, sizeof( int ), 1, file );
      fwrite( tmp_buff, sizeof( char ), bytecode.arg.len, file );
      return 0;
    default:
      return 1;
  }
}

/************************** End Code Generator **************************/ 
