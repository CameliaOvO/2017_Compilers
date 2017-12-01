/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"


/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4
#define MAX_SCOPE 1000

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

static Scope scopeExist[MAX_SCOPE];
static Scope scopeStack[MAX_SCOPE];
static int numScope = 0;
static int numScopeStack = 0;


Scope scope_create(char * name)
{
	Scope scope;
	scope = (Scope) malloc(sizeof(struct Scoperec));
	scope-> name = name;
	scope-> nestCount = numScopeStack;
	scope-> parent = scope_top();

	scopeExist[numScope++] = scope;
	return scope;
}


void scope_push(Scope scope)
{
	scopeStack[numScopeStack++] = scope;
}

void scope_pop()
{
	--numScopeStack;
}

Scope sc_top()
{
	return scopeStack[numScopeStack-1];
}

BucketList st_lookup_excluding_parent ( Scope scope, char * name)
{
	int h = hash(name);
	BucketList list = scope->bucket[h];
	while((l != NULL) && (strcmp(name, list->name)!= 0)){
		list = list->next;
	}
	if(list == NULL) return NULL;
	else return list;
}
BucketList st_lookup ( Scope scope, char * name )
{
	Scope s = scope;
	int h = hash(name);
	BucketList list = scope->bucket[h];
	
	while (s) {
		list = st_lookup_excluding_parent(s, name);
		if ( list != NULL ) return list;
		s = s->parent;
	}
	return NULL;

}


/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert(Scope scope, char * name, Exptype type, int lineno, int loc )
{ int h = hash(name);
  Scope top = scope_top();
  BucketList l =  top->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->type = type;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = top->bucket[h];
    top->bucket[h] = l; }
} /* st_insert */


/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Variable Name  Location   Line Numbers\n");
  fprintf(listing,"-------------  --------   ------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
        fprintf(listing,"%-8d  ",l->memloc);
        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
