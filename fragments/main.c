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
  variable[0].name           = malloc(10); strcpy(variable[0].name, "print |$|");
  variable[0].type           = 7;
  variable[0].vlen           = 1;
  variable[0].aval.argsI     = calloc(2, sizeof(int));
  variable[0].aval.argsI[0]  = 0;
  variable[0].aval.argsI[1]  = 0;
  variable[0].aval.argsT     = calloc(2, sizeof(int));
  variable[0].aval.argsT[0]  = 1;
  variable[0].aval.argsT[1]  = 2;
  variable[0].aval.argsL     = 2;
  variable[0].aval.operation = 1;
  
  printf("2Term v0.1.1i - IB2C interpreter\n");
  while (1)
  {
    for (i=0; i<256; i++) input[i] = '\0';
    printf("> ");
    gets(input);
    interpretInput(input);
  }
  return 0;
}

void interpretInput(char input[])
{
  int i=0, j=0, s=-1, n=0, t, quotes=0, isArg=0, fpnum=0, quitLoop=0;
  char segment[256];
  char arguments[10][256];
  char name[256];
  for (i=0; i<10; i++)
  {
    for (j=0; j<256; j++) arguments[i][j] = '\0';
  }
  
  j = 0;
  
  if (!skipWhiteSpace(&i, input)) return;
  
  for (i=0; i<256; i++)
  {                                                                                                //printf("------------\n>> i   = %d\n>> chr = %c\n", i, input[i]);
    if (isdigit(input[i]))
    {                                                                                              //puts(">> is_digit");
      if (!isArg)
      {                                                                                            //puts(">>> !isArg");
        if (++s == 10) break;
        j      = 0;
        quotes = 0;
        isArg  = 1;
      }
    }
    else
    {                                                                                              //puts(">> !is_digit");
      if (isArg && !quotes)
      {                                                                                            //puts(">>> is_number");
        if (input[i] == '.')
        {                                                                                          //puts(">>>> decimal");
          fpnum++;                                                                                 //printf(">>>> fpnum = %d\n", fpnum);
          if (fpnum == 2)
          {
            puts("ERR: Syntax: number has over 1 decimal point");
            break;
          }
        }
        else
        {                                                                                          //puts(">>>> end number");
          name[n++] = '|';
          name[n++] = (fpnum)? '%' : '#';
          name[n++] = '|';
          j     = 0;
          isArg = 0;
          fpnum = 0;
          continue;
        }
      }                                                                                            //puts(">>> switch");
      switch (input[i])
      {
        case '\0': quitLoop = 1; break;
        case ' ':
          if (!quotes)
          {                                                                                        //puts(">>>> space");
            skipWhiteSpace(&i, input);
            j         =   0;
            i--;
          }
          break;
        case '"':                                                                                  //puts(">>>> quotes");
          if (quotes)
          {                                                                                        //puts(">>>>> end");
            quotes    =   0;
            j         =   0;
            isArg     =   0;
            name[n++] = '|';
            name[n++] = '$';
            name[n++] = '|';
            continue;
          }
          else
          {                                                                                        //puts(">>>>> begin");
            if (++s == 10) { quitLoop = 1; break; }
            isArg  = 1;
            quotes = 1;
            j      = 0;
            i++;
          }
          break;
      }
      if (quitLoop) break;                                                                         //puts(">>> Didn't quit loop");
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
                                                                                                   //printf(">> argum: %s\n", arguments[s]);
  }
  
  if (isArg) // can only be number
  {
                                                                                                   puts(">> post-isArg");
    name[n++] = '|';
    name[n++] = (fpnum)? '%' : '#';
    name[n++] = '|';
  }
  for (n=n; n<256; n++) name[n] = '\0';
  
  argument = calloc(s, sizeof(Variable)); j = 0;                                            //puts(">> calloc'd argument");
  for (i=0; i<256; i++)
  {
    if (name[i] == '#')
    {
      argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|#|");
      argument[j].type = 1;
      argument[j].vlen = 1;
      argument[j].ival = atoi(arguments[j]);
      j++;
    }
    if (name[i] == '%')
    {
      argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|%|");
      argument[j].type = 2;
      argument[j].vlen = 1;
      argument[j].fval = atof(arguments[j]);
      j++;
    }
    if (name[i] == '$')
    {                                                                                              //puts(">> Detected string");
      argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|$|");
      argument[j].type = 3;
      argument[j].vlen = 1;
      argument[j].sval = (char*)malloc(strlen(arguments[j]+1));                                    //puts(">> malloc'd space");
      strcpy(argument[j].sval, arguments[j]);                                                      //puts(">> Set argument");
      j++;
    }
    if (name[i] == '\0') break;
  }
  
  puts(name);
  runFunc(name);
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
  int i, j, f=0, a=0;
  Variable args[4];
  //free(argument);
  
  for (i=0; i<var_count; i++)
  {
    if (strcmp(variable[i].name, name) == 0) 
    {
      f=1;
      break;
    }
  }
  if (f)
  {
    printf("%d\n", i);
    if (variable[i].type == 7)
    {
      switch (variable[i].aval.operation)
      {
        case 1:
          for (j=1; j<variable[i].aval.argsL; j++)
          {
            switch (variable[i].aval.argsT[j])
            {
              case 1: copyVariable(&args[a], variable[variable[i].aval.argsI[j]]); a++; break;
              case 2: copyVariable(&args[a], argument[variable[i].aval.argsI[j]]); a++; break;
            }
          }
          break;
      }
      runC(variable[i].aval.argsI[0], args);
    }      
    return;
  }
  puts("ERR: Function does not exist");
  return;
}

void copyVariable(Variable* dest, Variable src)
{
  dest->name = src.name;
  dest->type = src.type;
  dest->vlen = src.vlen;
  switch (src.type)
  {
    case 1: case 6: dest->ival = src.ival; break; //number, boolean
    case 2:         dest->fval = src.fval; break; //floating-point
    case 3: case 5: dest->sval = src.sval; break; //string, code
    case 4:         dest->pval = src.pval; break; //pointer
    case 7:         dest->aval = src.aval; break; //function
  }
  return;
}