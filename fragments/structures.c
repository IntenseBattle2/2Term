// >Terminal_ / 2Term - dual "shell" and language interpreter
// Copyright (C)2019 Intense Battle 2
// Please see the beginning comments of this program's main.c file for information on this program's license.

/* About this file (structures.c):
 * 
 * This file DOES NOT implement the structures used in >Terminal_. The actual implementation of the structures is done in 'structures.h'. This file defines
 * the functions that utilize the structures to create, modify, read, or destroy instances of them. This includes Variable creation/deletion, Variable
 * copying, type casting, Variable setting, function creation, Action interpretation, and the like. This file is NOT in charge of defining files that
 * dictate how the structures are used.
*/

#include "structures.h"

Variable* variable; unsigned int var_count;
Variable* argument; unsigned int arg_count;
int var_error;

unsigned int createVariable(char isTemp; const char name[], int type, unsigned int vlen, void contentArray[])
{
  var_error = 0; // Required to avoid error misfires
  unsigned int  i;
  Variable     *vArray;
  
  validate(name, type, contentArray, 1);
  if (var_error == 4) return 0;
  
  if (!isTemp) {
    i = findVariable(name);
    if (var_error == 3) 
    {
      puts("ERR: Memory: Variable already exists");
      var_err = -1;
      return i;
    }
    i = findVariable("<DELETED>");
    vArray = variable;
  }
  if (isTemp || var_error == 3)
  {
    vArray = (Variable*)realloc((isTemp)? argument : variable, (isTemp)? ++arg_count : ++var_count);
    if (!vArray) { puts("ERR: Internal: Could not allocate memory"); var_err = 1 + isTemp; return 0; }
    if(isTemp) {variable = vArray;} else { argument = vArray; }
    i = (isTemp)? arg_count : var_count;
  }
  
  vArray[i].name = name;
  vArray[i].type = type;
  vArray[i].vlen = vlen;
  switch (type)
  {
    case 1 || 4 || 7: vArray[i].sval =   (char*)contentArray; break;
    case 2:           vArray[i].ival =    (int*)contentArray; break;
    case 3:           vArray[i].fval = (double*)contentArray; break;
    case 5:           vArray[i].aval = (Action*)contentArray; break;
    case 6:           vArray[i].bval =  (FILE**)contentArray; break;
  }
  return i;
}

// Marks a variable as deleted and sets it to type void, content null.
// This is done because splicing it out of the array would take too much time and memory for a simple action. Instead, marking it as deleted is simple and
// can be taken advantage of later during variable creation to avoid having to reallocate `variable` so much, which has a chance of failing every time.
void deleteVariable(unsigned int index)
{
  variable[index].name = "<DELETED>";
  variable[index].vlen = 0;
  switch (variable[index].type)
  {
    case 1 || 4 || 7: free(variable[index].sval); break;
    case 2:           free(variable[index].ival); break;
    case 3:           free(variable[index].fval); break;
    case 5:           free(variable[index].aval); break;
    case 6:           free(variable[index].bval); break;
  }
  variable[index].type = 0;
  return;
}

void setVariable(unsigned int index, int type, unsigned int vlen, void contentArray[])
{
  validate(variable[index].name, type, contentArray, 0); //Making sure the variable's name doesn't include a type symbolism preventing this specific change
  if (var_error > 4 && var_error < 8) return; //AKA ERR:BADMATCHT, ERR:BADMATCHR, or ERR:READONLY. Just returning because validate already took care of them.
  
  variable[index].type = type;
  variable[index].vlen = vlen;
  switch (type)
  {
    case 1 || 4 || 7: variable[index].sval =   (char*)contentArray; break;
    case 2:           variable[index].ival =    (int*)contentArray; break;
    case 3:           variable[index].fval = (double*)contentArray; break;
    case 5:           variable[index].aval = (Action*)contentArray; break;
    case 6:           variable[index].bval =  (FILE**)contentArray; break;
  }
  return;
}

void castType(Variable* var, int new_type)
{
  void *ptr = 0x00; // A pointer pointing to 0x00000000 is recognized by `validate` as "Just keeping variable's content the same") 
  
  validate(var->name, new_type, ptr, 1);
  if (var_error == 5) return; // Only ERR:BADMATCHT (handled by `validate`) can be fired since we're not passing content and ignoring read-only status.
  
  switch (var->type)
  {
    case 0:
      var->type = new_type
      switch (new_type)
      {
        case 0:
          ptr = malloc(strlen(var->name)+1);
          strcpy((char*)ptr, var->name);
          deleteVariable(findVariable(var->name));
          strcpy(var->name, (char*)ptr);
          break;
        case 1:      var->sval =   (char*)calloc(1, sizeof(char));   break;
        case 2:      var->ival =    (int*)calloc(1, sizeof(int);     break;
        case 3:      var->fval = (double*)calloc(1, sizeof(double)); break;
        case 4 || 7: var->sval =   (char*)calloc(2, sizeof(char));   break;
        case 5:      var->aval = (Action*)calloc(1, sizeof(Action)); break;
        case 6:      var->bval =  (FILE**)calloc(1, sizeof(FILE**)); break;
      }
      break;
    case 1:
      switch (new_type)
      {
        case 0:
          var->type = new_type;
          free(var->sval);
          break;
        case 1: break;
        case 2:
          var->type    = new_type;
          var->ival    = (int*)malloc(sizeof(int));
          var->ival[0] = (int)var->sval;
          free(var->sval);
          break;
        case 3:
          var->type    = new_type;
          var->fval    = (double*)malloc(sizeof(double));
          var->fval[0] = (double)var->sval;
          free(var->sval);
          break;
        case 4:
          var->type    = new_type;
          ptr          = realloc(var->sval, 2);
          if (!ptr) { puts("ERR: Internal: Could not allocate memory"); var_err = 8; return; }
          var->sval    = (char*)ptr;
          var->sval[1] = '\0';
          free(ptr);
          break;
        case 5:
          puts("ERR: Typing: Cannot cast byte to function"); var_err = 9;
          return;
        case 6:
          puts("ERR: Typing: Cannot cast byte to streambuffer"); var_error = 9;
          return;
        case 7:
          var->type       = new_type;
          ptr             = malloc(3);
          ((char*)ptr)[0] = '\'';
          ((char*)ptr)[1] = var->svar[0];
          ((char*)ptr)[2] = '\'';
          free(var->svar);
          var->svar = (char*)ptr;
          return;
      }
    case 2:
      switch (new_type)
      {
        case 1:
          