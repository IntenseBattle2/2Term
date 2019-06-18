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

void interpretInput(char input[]);
int skipWhiteSpace(int* i, const char string[]);
void runFunc(const char name[]);
char* itostr(int number);

/*
   operation = 1 : run C function [arg0 = index, args... = argument(s)]
   operation = 2 : create permanent variable [arg1 = name, arg2 = type, arg3 = valueType, arg4 = value]
   operation = 3 : set permanent variable [arg1 = name, arg2 = type, arg3 = valueType, arg4 = value]
   operation = 4 : create temp variable [arg1 = index, arg2 = type, arg3 = valueType, arg4 = value]
   operation = 5 : set temp variable [arg1 = index, arg2 = type, arg3 = valueType, arg4 = value]
*/   
struct Action {
  struct Variable* argsV;
  int*             argsI;
  int*             argsT;
  int*             argsO;
  char             o_len;
  int              operation;
};

struct Variable {
  char*         name;
  int           type;
  int           ival;
  char*         sval;
  struct Action aval;
  void*         pval;
  double        fval;
};


struct Variable*  variable;
void**            function;
struct Variable*  argument;

int var_count;
int arg_count;


int main(int argc, char* argv[])
{
  char input[256];
  int  i;
  
  variable = calloc(1, sizeof(struct Variable));
  var_count = 1;
  variable[0].name           = malloc(10); strcpy(variable[0].name, "print |$|");
  variable[0].type           = 7;
  variable[0].aval.argsI     = calloc(2, sizeof(int));
  variable[0].aval.argsI[0]  = 0;
  variable[0].aval.argsI[1]  = 0;
  variable[0].aval.argsT     = calloc(2, sizeof(int));
  variable[0].aval.argsT[0]  = 1;
  variable[0].aval.argsT[1]  = 2;
  variable[0].aval.argsO     = calloc(2, 1);
  variable[0].aval.argsO[0]  = 2;
  variable[0].aval.argsO[1]  = 2;
  variable[0].aval.o_len     = 2;
  variable[0].aval.operation = 1;
  
  function = calloc(1, sizeof(void*));
  function[0] = &puts;
  
  printf("2Term v0.0.1i - IB2C interpreter\n");
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
  int i=0, j=0, s=0, n=0, t, quotes=0, isArg=0, fpnum=0, quitLoop=0;
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
  {                                                                                                puts("------------");
                                                                                                   printf(">> i   = %d\n", i);
                                                                                                   printf(">> chr = %c\n", input[i]);
    if (isdigit(input[i]))
    {
                                                                                                   puts(">> is_digit");
      if (!isArg)
      {
                                                                                                   puts(">>> !isArg");
        if (++s == 10) break;
        j      = 0;
        quotes = 0;
        isArg  = 1;
      }
    }
    else
    {                                                                                              puts(">> !is_digit");
      if (isArg && !quotes)
      {                                                                                            puts(">>> is_number");
        if (input[i] == '.')
        {                                                                                          puts(">>>> decimal");
          fpnum++;                                                                                 printf(">>>> fpnum = %d\n", fpnum);
          if (fpnum == 2)
          {
            puts("ERR: Syntax: number has over 1 decimal point");
            break;
          }
        }
        else
        {
                                                                                                   puts(">>>> end number");
          name[n++] = '|';
          name[n++] = (fpnum)? '%' : '#';
          name[n++] = '|';
          j     = 0;
          isArg = 0;
          fpnum = 0;
          continue;
        }
      }
                                                                                                   puts(">>> switch");
      switch (input[i])
      {
        case '\0': puts(">>> null"); quitLoop = 1; break;
        case ' ':
          if (!quotes)
          {                                                                                        puts(">>>> space");
            skipWhiteSpace(&i, input);
            j         =   0;
            i--;
          }
          break;
        case '"':
                                                                                                   puts(">>>> quotes");
          if (quotes)
          {                                                                                        puts(">>>>> end");
            quotes    =   0;
            j         =   0;
            isArg     =   0;
            name[n++] = '|';
            name[n++] = '$';
            name[n++] = '|';
            continue;
          }
          else
          {                                                                                        puts(">>>>> begin");
            if (++s == 10) { quitLoop = 1; break; }
            isArg  = 1;
            quotes = 1;
            j      = 0;
            i++;
          }
          break;
      }
      if (quitLoop) break;
                                                                                                   puts(">>> Didn't quit loop");
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
                                                                                                   printf(">> argum: %s\n", arguments[s]);
  }
  
  if (isArg) // can only be number
  {
    name[n++] = '|';
    name[n++] = (fpnum)? '%' : '#';
    name[n++] = '|';
  }
  for (n=n; n<256; n++) name[n] = '\0';
  
  /*
  argument = calloc(s, sizeof(struct Variable)); j = 0;
  for (i=0; i<256; i++)
  {
    if (name[i] == '#')
    {
      argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|#|");
      argument[j].type = 1;
      argument[j].ival = atoi(arguments[j++]);
    }
    if (name[i] == '%')
    {
      argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|%|");
      argument[j].type = 2;
      argument[j].fval = atof(arguments[j++]);
    }
    if (name[i] == '$')
    {
      argument[j].name = (char*)malloc(4); strcpy(argument[j].name, "|#|");
      argument[j].type = 3;
      argument[j].sval = (char*)malloc(strlen(arguments[j])+1); strcpy(argument[j].sval, arguments[j++]);
    }
    if (name[i] == '\0') break;
  }
  */
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
  int i;
  free(argument);
  
  for (i=0; i<var_count; i++)
  {
    if (strcmp(variable[i].name, name) == 0)
    {
      printf("%d\n", i);
      return;
    }
  }
  puts("ERR: Function does not exist");
  return;
}

char* itostr(int number)
{
  int i=0, j=0, newnum, subnum;
  char result[256];
  char* returnval;
  
  while (newnum > 0)
  {
    newnum  = number / pow(10,i);
    subnum  = newnum;
    subnum /= 10;
    subnum *= 10;
    newnum -= subnum;
    result[i++] = (char)(48 + newnum);
  }
  
  returnval = (char*)malloc(i--);
  for (i=i; i >= 0; i--) returnval[j++] = result[i];
  returnval[j] = '\0';
  
  return returnval;
}