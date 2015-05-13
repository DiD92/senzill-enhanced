/*************************************************************************** 
Stack Machine 
Author: Anthony A. Aaby
Modified by: Jordi Planes
***************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include "ST.h"
#include "SM.h"

/* OPERATIONS: External Representation */ 
char *op_name[] = {"halt", "store", "jmp_false", "goto", "call", "ret",
		   "data", "ld_int", "ld_real", "ld_var_i", "ld_var_r", "in_int", "in_real", 
       "out", "lt", "eq", "gt", "le", "ge", "ne", "and", "or", "not", 
       "add", "sub", "mult", "div", "pwr", "neg" }; 

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

/*========================================================================= 
  Operations
  =========================================================================*/ 
  stack_elem operate_terms(stack_elem s1, stack_elem s2, int op)
  {
    stack_elem res;
    int iv1, iv2;
    float fv1, fv2;
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
      case WRITE:
        switch(stack[top].v_type) {
          case T_INTEGER:
            printf( "Output: %d\n", stack[top--].iv );
            break;
          case T_REAL:
            printf( "Output: %4.4f\n", stack[top--].rv );
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
          default:
            printf( "%d Internal Error: Memory Dump\n", ir.op ); 
            break; 
        }
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
      case LD_VAR_I: 
        stack[++top].iv = stack[ar+ir.arg.iv].iv;
        stack[top].v_type = stack[ar+ir.arg.iv].v_type;
        break;
      case LD_VAR_R:
        stack[++top].rv = stack[ar+ir.arg.iv].rv;
        stack[top].v_type = stack[ar+ir.arg.iv].v_type;
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
        s2 = stack[top];
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
      default: 
        printf( "%d Internal Error: Memory Dump\n", ir.op ); 
        break; 
    } 
  } while (ir.op != HALT); 
} 
/*************************** End Stack Machine **************************/ 
