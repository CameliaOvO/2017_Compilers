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
#include "globals.h"
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
  scope = (Scope) malloc(sizeof(struct ScopeListRec));
  scope-> name = name;
  scope-> nestCount = numScopeStack;
  scope-> parent = scope_top();
  scope-> scopeLoc = 0;

  scopeExist[numScope++] = scope;
  return scope;
}


void scope_push(Scope scope)
{
  scopeStack[numScopeStack++] = scope;
}

void scope_pop(int endLine)
{
  Scope s = scope_top();
  if (endLine != -1) {
    char * newName = (char*)malloc(sizeof(s->name)+12);
    sprintf(newName, "%s~%d",s->name, endLine);
    s -> name = newName;
  }
  --numScopeStack;
}

Scope scope_top()
{
  return scopeStack[numScopeStack-1];
}

BucketList st_bucket( char * name )
{
  int h = hash(name);
  Scope s = scope_top();
  while (s) {
    BucketList list = s->bucket[h];
    while((list != NULL) && (strcmp(name, list->name)!= 0)){
      list = list->next;
      }
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
void st_insert( char * name, int lineno, TreeNode * treeNode )
{ int h = hash(name);
  Scope top = scope_top();
  BucketList l =  top->bucket[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->treeNode = treeNode;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = top->scopeLoc++;
    l->lines->next = NULL;
    l->next = top->bucket[h];
    top->bucket[h] = l; }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found */
int st_lookup ( char * name )
{ BucketList l = st_bucket(name);
  if (l != NULL) return l->memloc;
  return -1;
}

int st_exist_top (char * name)
{ int h = hash(name);
  Scope sc = scope_top();
  while(sc) {
    BucketList l = sc->bucket[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
      l = l->next;
    if (l != NULL) return TRUE;
    break;
  }
  return FALSE;
}

void st_add_lineno(char * name, int lineno)
{ BucketList l = st_bucket(name);
  LineList ll = l->lines;
  while (ll->next != NULL) ll = ll->next;
  ll->next = (LineList) malloc(sizeof(struct LineListRec));
  ll->next->lineno = lineno;
  ll->next->next = NULL;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i, j;

  for (i = 0; i < numScope; ++i) {

    Scope scope = scopeExist[i];
    BucketList * bucket = scope->bucket;

    char* scopeName = i == 0 ? "" : scope->name;
  fprintf(listing, "Scope name : ~");
    if(i != 0){
      fprintf(listing, ":%s", scopeName);

    }
    fprintf(listing, "\n");
  fprintf(listing,"----------------------------------------------------\n");

  fprintf(listing,"Variable Name Variable Type  Location   Line Numbers\n");
  fprintf(listing,"------------- -------------  --------   ------------\n");


  for (j=0;j<SIZE;++j)
  { if (bucket[j] != NULL)
    { BucketList l = bucket[j];
      TreeNode *node = l->treeNode;

      while (l != NULL)
      { LineList t = l->lines; 
        fprintf(listing,"%-14s",l->name);
        switch (node->type) {
        case Void:
          fprintf(listing, "Void           ");
          break;
        case Integer:
          fprintf(listing, "Integer        ");
          break;
        default:
          break;
        }

        fprintf(listing,"%-8d ",l->memloc);

        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
    fprintf(listing,"----------------------------------------------------\n\n");  
}
} /* printSymTab */
