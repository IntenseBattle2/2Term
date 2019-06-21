// >Terminal_ / 2Term - dual "shell" and language interpreter
// Copyright (C)2019 Intense Battle 2
// Please see the beginning comments of this program's main.c file for information on this program's license.

/* About this file (structures.h)
 *
 * This file implements the types `struct Action` and `struct Variable` along with their typedefs
*/

#ifndef HEADER_STRUCTURES
#define HEADER_STRUCTURES

/*
   operation = 1 : run C function [arg0 = index, args... = argument(s)]
   operation = 2 : create permanent variable [arg1 = name, arg2 = type, arg3 = valueType, arg4 = value]
   operation = 3 : set permanent variable [arg1 = name, arg2 = type, arg3 = valueType, arg4 = value]
   operation = 4 : create temp variable [arg1 = index, arg2 = type, arg3 = valueType, arg4 = value]
   operation = 5 : set temp variable [arg1 = index, arg2 = type, arg3 = valueType, arg4 = value]
*/   

typedef struct {
  int*      argsI;
  int*      argsT;
  int*      argsO;
  char      argsL;
  int       operation;
} Action;

typedef struct {
  char*        name;
  int          type;
  unsigned int vlen;
  int          ival;
  char*        sval;
  Action       aval;
  void*        pval;
  double       fval;
} Variable;

#endif