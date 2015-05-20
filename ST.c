/*************************************************************************** 
Symbol Table Module 
Author: Anthony A. Aaby
Modified by: Jordi Planes
***************************************************************************/ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CG.h"
#include "ST.h"

/*========================================================================= 
DECLARATIONS 
=========================================================================*/ 

/*------------------------------------------------------------------------- 
SYMBOL TABLE ENTRY 
-------------------------------------------------------------------------*/ 
symrec *identifier; 

/*------------------------------------------------------------------------- 
SYMBOL TABLE 
Implementation: a chain of records. 
------------------------------------------------------------------------*/ 
// REPLACE BY sym_stack in ST.h
symrec *sym_table = (symrec *)0; /* The pointer to the Symbol Table */ 

/*======================================================================== 
  Operations: Putsym, Getsym 
  ========================================================================*/
symrec * putsym_e (char *sym_name, int sym_type, int sym_const, int sym_len )
{
  symrec *ptr; 
  ptr = (symrec *) malloc (sizeof(symrec)); 
  ptr->name = (char *) malloc (strlen(sym_name)+1); 
  strcpy (ptr->name,sym_name);
  ptr->type = sym_type;
  ptr->cnst = sym_const;
  ptr->offset = data_location();
  ptr->cnt_len = sym_len; 
  ptr->next = (struct symrec *)sym_table; 
  sym_table = ptr;
  data_offset += sym_len; 
  return ptr; 
} 
symrec * putsym (char *sym_name, int sym_type, int sym_const ) 
{
  return putsym_e(sym_name, sym_type, sym_const, 0);
} 

symrec * getsym (char *sym_name) 
{ 
  symrec *ptr; 
  for ( ptr = sym_table; ptr != (symrec *) 0; ptr = (symrec *)ptr->next ) 
    if (strcmp (ptr->name,sym_name) == 0) 
      return ptr; 
  return NULL;
} 
/************************** End Symbol Table **************************/ 

