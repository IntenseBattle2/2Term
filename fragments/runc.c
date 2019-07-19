// >Terminal_ / 2Term - dual "shell" and language interpreter
// Copyright (C)2019 Intense Battle 2
// Please see the beginning comments of this program's main.c file for information on this program's license.

/* About this file (runc.c):
 *
 * This file is stored seperately due to how long it's expected to get. This file contains all the necessary functions to run the function runC, including runC
 * itself. runC executes functions by its memory address. runC also returns the function return in the `struct Variable` type. Its code, as well as its
 * function's code, is basically a long copy-paste-esque list. Since every function takes its own number of arguments with their own types, each function for the
 * most part has to be individually dealt with. Any overlaps in both argument count and types are linked together in the calling functions. Otherwise, all
 * functions are seperate. And considering listing every single function this program has is what's planned, yeah, this is going to get LOOOOOOONG.
*/

#include "structures.h"

Variable retvar;
int      tpinfo[5];

Variable runC(int index, Variable args[4])
{
  switch (index){
    case 0: retvar.ival = (int*)malloc(sizeof(int)); *retvar.ival = puts(args[0].sval); break; //int puts(const char *str)
  }
  return retvar;
}