// >Terminal_ / 2Term - dual "shell" and language interpreter
// Copyright (C)2019 Intense Battle 2
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public license as published by
 * the Free Software Foundation, either version 3 of the license, or
 * (at your option) any later version.
 *
 * This program is distributed in the hopes that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses>.
 *
*/
// The project handler for 2Term is Colin "ColinTNMP" Breese. You may contact him about 2Term at <IB2Member01@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"
#include "runc.h"

void interpretInput(char input[]);
int skipWhiteSpace(int* i, const char string[]);
void runFunc(const char name[]);
void copyVariable(Variable* dest, Variable src);

Variable*  variable;
Variable*  argument;

int var_count;
int arg_count;


int main(int argc, char* argv[])
{
  char input[256];
  int  i;
  
  variable = calloc(1, sizeof(Variable));
  var_count = 1;
  variable[0].name              = (char*)malloc(10); strcpy(variable[0].name, "print |$|");
  variable[0].type              = 5;
  variable[0].vlen              = 1;
  variable[0].aval              = (Action*)malloc(sizeof(Action));
  variable[0].aval[0].argsI     = calloc(2, sizeof(int));
  variable[0].aval[0].argsI[0]  = 0;
  variable[0].aval[0].argsI[1]  = 0;
  variable[0].aval[0].argsT     = calloc(2, sizeof(int));
  variable[0].aval[0].argsT[0]  = 1;
  variable[0].aval[0].argsT[1]  = 2;
  variable[0].aval[0].argsL     = 2;
  variable[0].aval[0].operation = 1;
  
  printf("2Term v0.1.4i - IB2C interpreter\n");
  while (1)
  {
    for (i=0; i<256; i++) input[i] = '\0';
    printf("> ");
    gets(input);
    interpretInput(input);                                                                         puts("Exited interpretInput");
  }
  return 0;
}

void interpretInput(char input[])
{
  int i, j, s=-1, n=0, t, quotes=0, isArg=0, fpnum=0, quitLoop=0;
  char segment[256], arguments[10][256], name[256] argIsVar[10]={0,0,0,0,0,0,0,0,0,0};
  for (i=0; i<10; i++) 
    for (j=0; j<256; j++)
      arguments[i][j] = '\0';
  j = 0;
  
  if (!skipWhiteSpace(&i, input)) return;
  
  for (i=0; i<256; i++)
  {
    if (isdigit(input[i]))
    {
      if (!isArg)
      {
        if (++s == 10) break;
        j      = 0;
        quotes = 0;
        isArg  = 1;
      }
    }
    else
    {
      if (isArg && !quotes)
      {
        if (input[i] == '.')
        {
          fpnum++;
          if (fpnum == 2) { puts("ERR: Syntax: number has over 1 decimal point"); break; }
        }
        else
        {
          name[n++] = '|';
          name[n++] = (fpnum)? '%' : '#';
          name[n++] = '|';
          j         =   0;
          isArg     =   0;
          fpnum     =   0;
        }
      }
      switch (input[i])
      {
        case '\0': quitLoop = 1; break;
        case ' ':
          if (!quotes)
          {
            segment[j] = '\0';
            t = findVariable(segment);
            if (t != -1)
            {
              if(++s == 10) { quitLoop = 1; break; }
              strcpy(arguments[s], segment);
              argIsVar[s] = 1;
              name[n++] = '|';
              switch (variable[t].type)
              {
                case 0: name[n++] = '0'; break;
                case 1: name[n++] = '^'; break;
                case 2: name[n++] = '#'; break;
                case 3: name[n++] = '%'; break;
                case 4: name[n++] = '$'; break;
                case 5: /*not possible*/ break;
                case 6: name[n++] = '6'; break;
                case 7: name[n++] = '*'; break;
                /* get remainder of numbers */
              }
              name[n++] = '|';
            skipWhiteSpace(&i, input);
            j=0;
            i--;
          }
          break;
        case '"':
          if (quotes)
          {
            quotes    =   0;
            j         =   0;
            isArg     =   0;
            name[n++] = '|';
            name[n++] = '$';
            name[n++] = '|';
            continue;
          }
          else
          {
            if (++s == 10) { quitLoop = 1; break; }
            isArg  = 1;
            quotes = 1;
            j      = 0;
            i++;
          }
          break;
      }
      if (quitLoop) break;
    }
    
    if (isArg)
    {
      arguments[s][j++] = input[i];
    }
    else
    {
      segment[j++] = input[i];
      name[n++]    = input[i];
    }
  }
  
  for (n=n; n<256; n++) name[n] = '\0';
  
  argument = calloc(s, sizeof(Variable)); j = 0;
  for (i=0; i<256; i++)
  {
    if (name[i] == '\0') break;
    switch (name[i])
    {
      case '#':
        if (argIsVar[j])
        {
          t = findVariable(arguments[j]);
          copyVariable(&argument[j], variable[t]);
        }
        else
        {
          argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|#|");
          argument[j].type = 2;
          argument[j].vlen = 1;
          argument[j].ival    = (int*)malloc(sizeof(int));
          argument[j].ival[0] = atoi(arguments[j]);
        }
        j++;
        break;
      case '%':
        if (argIsVar[j])
        {
          t = findVariable(arguments[j]);
          copyVariable(&argument[j], variable[t]);
        }
        else
        {
          argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|%|");
          argument[j].type = 2;
          argument[j].vlen = 1;
          argument[j].fval = (double*)malloc(sizeof(double));       argument[j].fval[0] = atof(arguments[j]);
        }
        j++;
        break;
      case '$':
        if (argIsVar[j])
        {
          t = findVariable(arguments[j]);
          copyVariable(&argument[j], variable[t]);
        }
        else
        {
          argument[j].name = (char*)malloc(4);                      strcpy(argument[j].name, "|$|");
          argument[j].type = 3;
          argument[j].vlen = 1;
          argument[j].sval = (char*)malloc(strlen(arguments[j]+1)); strcpy(argument[j].sval, arguments[j]);
        }
        j++;
        break;
       // The following cases would only show up if a variable was passed, so we know we have to do a copyVariable call.
       // In the event one of the passed arguments was supposed to be one of these types, but wasn't passed as a variable, we let runFunc handle it.
       case '0' || '1' || '2' || '3' || '4' || '5' || '6' || '7' || '8' || '9': //It's gotta be a non-symbol-bound type, doesn't quite matter which
       case '^' || '&' || '*': //Not at all prelevant, but I find it funny these happen to be right next to each other on the keyboard :P
         t = findVariable(arguments[j]);
         copyVariable(&argument[j], variable[t]);
         j++;
       break;
    }
  }
  
  puts(name);
  runFunc(name);                                                                                   puts("Ran function");
  free(argument);                                                                                  puts("Freed argument");
  return;
}

int skipWhiteSpace(int* i, const char string[])
{
  int ni = *i;
  while (string[ni++] == ' ') {}
  *i = ni - 1;
  if (string[ni] == '\0') return 0;
  return 1;
}

void runFunc(const char name[])
{
  int i, j, f=-1, a=0;
  Variable args[4];
  
  for (i=0; i<var_count; i++)
    if ( !strcmp(variable[i].name, name) ) { f=i; break; }
  
  if (f == -1) { puts("ERR: Reference: function doesn't exist"); return; }
  
  printf("%d\n", i);
  if (variable[f].type != 5) { puts("ERR: Typing: Not a function"); return; }
  
  for (i=0; i<variable[f].vlen; i++)
  {
    switch (variable[f].aval[i].operation)
    {
      case 1:
        for (j=1; j<variable[f].aval[i].argsL; j++)
          switch (variable[f].aval[i].argsT[j])
          {
            case 1: copyVariable(&args[a], variable[variable[f].aval[i].argsI[j]]); a++; break;
            case 2: copyVariable(&args[a], argument[variable[f].aval[i].argsI[j]]); a++; break;
          }
        break;
    }
    runC(variable[f].aval[i].argsI[0], args);
  }
  return;
}

void copyVariable(Variable* dest, Variable src)
{
  dest->name = src.name;
  dest->type = src.type;
  dest->vlen = src.vlen;
  switch (src.type)
  {
    case 1: case 2: dest->ival = src.ival; break; //byte, integer
    case 3:         dest->fval = src.fval; break; //floating-point
    case 4: case 7: dest->sval = src.sval; break; //string, raw
    case 8:         dest->pval = src.pval; break; //pointer
    case 5:         dest->aval = src.aval; break; //function
    case 6:         dest->bval = src.bval; break; //streambuffer (FILE*)
  }
  return;
}