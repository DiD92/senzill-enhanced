/*************************************************************************** 
Stack Machine 
Author: Anthony A. Aaby
Modified by: Jordi Planes
***************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ST.h"
#include "SM.h"

/* OPERATIONS: External Representation */ 
char *op_name[] = {"halt", "store", "sti", "jmp_false", "goto", "call", "ret",
		   "data", "ld_int", "ld_real", "ld_str", "ld_var_i", "ld_var_r", "ld_var_s",
       "ld_ptr", "ldi", "in_int", "in_real", "in_str", "out", "lt", "eq", "gt", "le", "ge", 
       "ne", "and", "or", "not", "add", "sub", "mult", "div", "pwr", "neg", "slen" }; 

/* CODE Array */ 
struct instruction code[MAX_MEMORY];

/* RUN-TIME Stack */ 
stack_elem stack[MAX_MEMORY];

/*------------------------------------------------------------------------- 
  Registers 
  -------------------------------------------------------------------------*/ 
int pc = 0; 
struct instruction ir; 
int ar = 0; 
int top = 0; 
char ch;

/*========================================================================= 
  Generate Stack Element
  =========================================================================*/ 
stack_elem * gen_stack_elem_i(int value)
{
  stack_elem *ptr;
  ptr = (stack_elem *) malloc(sizeof(stack_elem));
  ptr->v_type = T_INTEGER;
  ptr->iv = value;
  return ptr;
}

stack_elem * gen_stack_elem_r(float value)
{
  stack_elem *ptr;
  ptr = (stack_elem *) malloc(sizeof(stack_elem));
  ptr->v_type = T_REAL;
  ptr->rv = value;
  return ptr;
}

stack_elem * gen_stack_elem_s(char *value)
{
  stack_elem *ptr;
  ptr = (stack_elem *) malloc(sizeof(stack_elem));
  ptr->v_type = T_STRING;
  ptr->str = value;
  ptr->len = strlen(value);
  return ptr;
}

stack_elem * gen_stack_elem_p(int base, int span)
{
  stack_elem *ptr;
  ptr = (stack_elem *) malloc(sizeof(stack_elem));
  ptr->v_type = T_POINTER;
  ptr->base = base;
  ptr->span = span;
  return ptr;
}

/*========================================================================= 
  Operations
  =========================================================================*/ 
  stack_elem operate_terms(stack_elem s1, stack_elem s2, int op)
  {
    stack_elem res;
    int iv1, iv2;
    float fv1, fv2;
    char *stmp;
    switch(s1.v_type) {
      case T_INTEGER: // First value is INTEGER
        switch(s2.v_type) {
          case T_INTEGER: // Second value is INTEGER
            res.v_type = T_INTEGER;
            iv1 = s1.iv;
            iv2 = s2.iv;
            switch(op) {
              case LT:
                res.iv = iv1 < iv2;
                break;
              case EQ:
                res.iv = iv1 == iv2;
                break;
              case GT:
                res.iv = iv1 > iv2;
                break;
              case LE:
                res.iv = iv1 <= iv2;
                break;
              case GE:
                res.iv = iv1 >= iv2;
                break;
              case NE:
                res.iv = iv1 != iv2;
                break;
              case ADD:
                res.iv = iv1 + iv2;
                break;
              case SUB:
                res.iv = iv1 - iv2;
                break;
              case MULT:
                res.iv = iv1 * iv2;
                break;
              case DIV:
                res.iv = iv1 / iv2;
                break;
              case PWR:
                res.iv = iv1 * iv2; //TOFIX
                break;
              default:
                printf( "%d Internal Error: Memory Dump\n", ir.op );
                break;
            }
            break;
          case T_REAL: // Second value is REAL
            res.v_type = T_REAL;
            iv1 = s1.iv;
            fv2 = s2.rv;
            switch(op) {
              case LT:
                res.iv = iv1 < fv2;
                break;
              case EQ:
                res.iv = iv1 == fv2;
                break;
              case GT:
                res.iv = iv1 > fv2;
                break;
              case LE:
                res.iv = iv1 <= fv2;
                break;
              case GE:
                res.iv = iv1 >= fv2;
                break;
              case NE:
                res.iv = iv1 != fv2;
                break;
              case ADD:
                res.rv = iv1 + fv2;
                break;
              case SUB:
                res.rv = iv1 - fv2;
                break;
              case MULT:
                res.rv = iv1 * fv2;
                break;
              case DIV:
                res.rv = iv1 / fv2;
                break;
              case PWR:
                res.rv = iv1 * fv2; //TOFIX
                break;
              default:
                printf( "%d Internal Error: Memory Dump\n", ir.op );
                break;
            }
            break;
          case T_STRING: // Second value is STRING
            if( MULT == ir.op ) {
              if( s1.iv > 0 ) {
                iv1 = s2.len * s1.iv;
                iv2 = 1;
                stmp = malloc( sizeof(char) * iv1 );
                strcpy( stmp, s2.str );
                while( iv2 < s1.iv ) {
                  strcat( stmp, s2.str );
                  iv2++;
                }
                res.str = stmp;
                res.len = iv1;
              } else {
                res.str = "";
                res.len = 0;
              }
            } else {
              printf( "%d %d %d Incompatible types for operation\n", ir.op, s1.v_type, s2.v_type );
            }
            break;
          default:
            printf( "%d Internal Error: Memory Dump\n", ir.op );
            break;
        }
        break;
      case T_REAL: // First value is REAL
        switch(s2.v_type) {
          case T_INTEGER: // Second value is INTEGER
            res.v_type = T_REAL;
            fv1 = s1.rv;
            iv2 = s2.iv;
            switch(op) {
              case LT:
                res.iv = fv1 < iv2;
                break;
              case EQ:
                res.iv = fv1 == iv2;
                break;
              case GT:
                res.iv = fv1 > iv2;
                break;
              case LE:
                res.iv = fv1 <= iv2;
                break;
              case GE:
                res.iv = fv1 >= iv2;
                break;
              case NE:
                res.iv = fv1 != iv2;
                break;
              case ADD:
                res.rv = fv1 + iv2;
                break;
              case SUB:
                res.rv = fv1 - iv2;
                break;
              case MULT:
                res.rv = fv1 * iv2;
                break;
              case DIV:
                res.rv = fv1 / iv2;
                break;
              case PWR:
                res.rv = fv1 * iv2; //TOFIX
                break;
              default:
                printf( "%d Internal Error: Memory Dump\n", ir.op );
                break;
            }
            break;
          case T_REAL: // Second value is REAL
            res.v_type = T_REAL;
            fv1 = s1.rv;
            fv2 = s2.rv;
            switch(op) {
              case LT:
                res.iv = fv1 < fv2;
                break;
              case EQ:
                res.iv = fv1 == fv2;
                break;
              case GT:
                res.iv = fv1 > fv2;
                break;
              case LE:
                res.iv = fv1 <= fv2;
                break;
              case GE:
                res.iv = fv1 >= fv2;
                break;
              case NE:
                res.iv = fv1 != fv2;
                break;
              case ADD:
                res.rv = fv1 + fv2;
                break;
              case SUB:
                res.rv = fv1 - fv2;
                break;
              case MULT:
                res.rv = fv1 * fv2;
                break;
              case DIV:
                res.rv = fv1 / fv2;
                break;
              case PWR:
                res.rv = fv1 * fv2; //TOFIX
                break;
              default:
                printf( "%d Internal Error: Memory Dump\n", ir.op );
                break;
            }
            break;
          default:
            printf( "%d Internal Error: Memory Dump\n", ir.op );
            break;
        }
        break;
      case T_STRING: // First value is STRING
        res.v_type = T_STRING;
        switch(s2.v_type) {
          case T_STRING: // Second value is STRING
            if( ADD == ir.op ) {
              iv1 = s1.len + s2.len;
              stmp = malloc( sizeof(char) * iv1 );
              strcpy( stmp, s1.str );
              strcat( stmp, s2.str );
              res.str = stmp;
              res.len = iv1;
            } else {
              printf( "%d %d %d Incompatible types for operation\n", ir.op, s1.v_type, s2.v_type );
            }
            break;
          case T_INTEGER: // Second value is INTEGER
            if( MULT == ir.op ) {
              if( s2.iv > 0 ) {
                iv1 = s1.len * s2.iv;
                iv2 = 1;
                stmp = malloc( sizeof(char) * iv1 );
                strcpy( stmp, s1.str );
                while( iv2 < s2.iv ) {
                  strcat( stmp, s1.str );
                  iv2++;
                }
                res.str = stmp;
                res.len = iv1;
              } else {
                res.str = "";
                res.len = 0;
              }
            } else {
              printf( "%d %d %d Incompatible types for operation\n", ir.op, s1.v_type, s2.v_type );
            }
            break;
          default:
            printf( "%d %d %d Incompatible types for operation\n", ir.op, s1.v_type, s2.v_type );
            break;
        }
        break;
      default:
        printf( "%d Internal Error: Memory Dump\n", ir.op );
        break;
    }
    return res;
  }

/*========================================================================= 
  Fetch Execute Cycle 
  =========================================================================*/ 
void fetch_execute_cycle() 
{
  stack_elem s1, s2, res;
  int tmp;
  char *buff;
  do { 
#ifndef NDEBUG    
    printf( "PC = %3d IR.arg = %8d AR = %3d Top = %3d,%8d\n", 
	    pc, ir.arg, ar, top, stack[top]); 
#endif
    /* Fetch */ 
    ir = code[pc++]; 
    /* Execute */ 
    switch (ir.op) { 
      case HALT:
        printf( "Halt\n" ); 
        break; 
      case READ_INT:
        printf( "Input integer: " );
        scanf( "%d", &stack[ar+ir.arg.iv].iv );
        stack[ar+ir.arg.iv].v_type = T_INTEGER;
        break;
      case READ_REAL:
        printf( "Input real: " );
        scanf( "%f", &stack[ar+ir.arg.iv].rv );
        stack[ar+ir.arg.iv].v_type = T_REAL;
        break;
      case READ_STR:
        printf( "Input string: ");
        buff = malloc(MAX_CHARS);
        fgets( buff, MAX_CHARS, stdin );
        tmp = strlen( buff );
        if ( ( tmp > 0 ) && ( buff[tmp - 1] == '\n')) {
          buff[tmp - 1] = '\0';
        }
        stack[ar+ir.arg.iv].len = tmp - 1;
        stack[ar+ir.arg.iv].str = buff;
        stack[ar+ir.arg.iv].v_type = T_STRING;
        break;
      case WRITE:
        switch(stack[top].v_type) {
          case T_INTEGER:
            printf( "Output: %d\n", stack[top--].iv );
            break;
          case T_REAL:
            printf( "Output: %4.4f\n", stack[top--].rv );
            break;
          case T_STRING:
            printf( "Output: %s\n", stack[top--].str );
            break;
          default:
            printf( "%d Internal Error: Memory Dump\n", ir.op ); 
            break; 
        }
        break;
      case STORE:
        switch(stack[top].v_type) {
          case T_INTEGER:
            stack[ir.arg.iv].iv = stack[top].iv;
            stack[ir.arg.iv].v_type = stack[top--].v_type;
            break;
          case T_REAL:
            stack[ir.arg.iv].rv = stack[top].rv;
            stack[ir.arg.iv].v_type = stack[top--].v_type;
            break;
          case T_STRING:
            stack[ir.arg.iv].str = strdup( stack[top].str );
            stack[ir.arg.iv].len = stack[top].len;
            stack[ir.arg.iv].v_type = stack[top--].v_type;
            break;
          default:
            printf( "%d Internal Error: Memory Dump\n", ir.op ); 
            break; 
        }
        break;
      case STI:
        switch(stack[top].v_type) {
          case T_INTEGER:
            stack[stack[top-1].iv].iv = stack[top].iv;
            stack[stack[top-1].iv].v_type = stack[top].v_type;
            break;
          case T_REAL:
            stack[stack[top-1].iv].rv = stack[top].rv;
            stack[stack[top-1].iv].v_type = stack[top].v_type;
            break;
          case T_STRING:
            stack[stack[top-1].iv].str = strdup( stack[top].str );
            stack[stack[top-1].iv].len = stack[top].len;
            stack[stack[top-1].iv].v_type = stack[top].v_type;
            break;
          default:
            printf( "%d Internal Error: Memory Dump\n", ir.op ); 
            break; 
        }
        top = top - 2;
        break;
      case JMP_FALSE: 
        if ( stack[top--].iv == 0 ) 
          pc = ir.arg.iv; 
        break; 
      case GOTO: 
        pc = ir.arg.iv; 
        break; 
      case CALL: 
        stack[++top].iv = pc+1; 
        pc = ir.arg.iv; 
        break;
      case RET: 
        pc = stack[top--].iv; 
        break;
      case DATA: 
        top = top + ir.arg.iv; 
        break; 
      case LD_INT: 
        stack[++top].v_type = T_INTEGER;
        stack[top].iv = ir.arg.iv; 
        break;
      case LD_REAL:
        stack[++top].v_type = T_REAL;
        stack[top].rv = ir.arg.rv;
        break;
      case LD_STR:
        stack[++top].v_type = T_STRING;
        tmp = ir.arg.len;
        if( tmp > 2 ) {
          stack[top].str = strndup( &ir.arg.str[1], tmp - 2 );
          stack[top].len = tmp - 2;
        } else {
          stack[top].str = "";
          stack[top].len = 0;
        }
        break;
      case LD_VAR_I: 
        stack[++top].iv = stack[ar+ir.arg.iv].iv;
        stack[top].v_type = stack[ar+ir.arg.iv].v_type;
        break;
      case LD_VAR_R:
        stack[++top].rv = stack[ar+ir.arg.iv].rv;
        stack[top].v_type = stack[ar+ir.arg.iv].v_type;
        break;
      case LD_VAR_S:
        stack[++top].str = strdup( stack[ar+ir.arg.iv].str );
        stack[top].len = stack[ar+ir.arg.iv].len;
        stack[top].v_type = stack[ar+ir.arg.iv].v_type;
        break;
      case LD_PTR:
        stack[++top].base = ir.arg.base;
        stack[top].span = ir.arg.span;
        stack[top].v_type = ir.arg.v_type;
        break;
      case LDI:
        stack[top] = stack[stack[top].iv];
        break;
      case LT:
      case EQ:
      case GT:
      case LE:
      case GE:
      case NE:
        if ( operate_terms( stack[top-1], stack[top], ir.op ).iv )
          stack[--top].iv = 1; 
        else 
          stack[--top].iv = 0; 
        break;
      case AND:
        tmp = ( ( stack[top-1].iv ) && ( stack[top].iv ) );
        stack[--top].iv = tmp;
        break;
      case OR:
        tmp = ( ( stack[top-1].iv ) || ( stack[top].iv ) );
        stack[--top].iv = tmp;
        break;
      case NOT:
        ( stack[top].iv == 1 ) ? ( stack[top].iv = 0 ) : ( stack[top].iv = 1 );
        break;
      case ADD:
      case SUB:
      case MULT:
      case DIV:
      case PWR:
        s1 = stack[top-1];
        if(s1.v_type == T_POINTER) {
          s1.iv = s1.base;
          s1.v_type = T_INTEGER;
        }
        s2 = stack[top];
        if(s2.v_type == T_POINTER) {
          s2.iv = s2.base;
          s2.v_type = T_INTEGER;
        }
        res = operate_terms( s1, s2, ir.op ); // ERRORS
        stack[top-1] = res;
        top--;
        break;
      case NEG:
        s1 = stack[top];
        switch (s1.v_type) {
          case T_INTEGER:
            stack[top].iv = -1 * s1.iv;
            break;
          case T_REAL:
            stack[top].rv = -1.0 * s1.rv;
            break;
          default:
            printf( "%d Internal Error: Memory Dump\n", ir.op ); 
            break;
        }
        break;
      case SLEN:
        switch (stack[top].v_type) {
          case T_STRING:
            stack[top].iv = stack[top].len;
            break;
          case T_POINTER:
            stack[top].iv = stack[top].span;
            break;
          default:
            stack[top].iv = 0;
            break;
        }
        stack[top].v_type = T_INTEGER;
        break;
      default: 
        printf( "%d Internal Error: Memory Dump\n", ir.op ); 
        break; 
    } 
  } while (ir.op != HALT); 
} 
/*************************** End Stack Machine **************************/ 
