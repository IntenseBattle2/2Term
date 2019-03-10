//>Terminal_ / 2Term - dual-purpose terminal "shell" and script interpreter
//Copyright (C)2018-2019 Intense Battle 2
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
// readline.c - implements readline() for interpreting C code

/* About this file: readline.c
 * 
 * This file defines functions for reading code entered
 * into IB2C's `RunC` command along with helper functions
 * and some data for keeping track of the functions' info.
 *
 * For running the code, see runInstruction.c; for initial
 * parsing of `RunC`'s content into lines, sit tight!!!!!
*/

#include "datastore.c"

/* File dependencies:
 *
 * datastore.c
 * |struct Instruction
 * ||struct Variable
 * |||enum DataType
 * |||union Content
 * |struct Variable
 * ||enum DataType
 * ||union Content
 * |struct Macro
 * ||struct Variable
 * |||enum DataType
 * |||union Content
*/

/*
 * ||================||
 * || Reference Data ||
 * ||================||
*/

const char* keywords[32] = {
  "auto",     "break",    "case",     "char",
  "const",    "continue", "default",  "do",
  "double",   "else",     "enum",     "extern",
  "float",    "for",      "goto",     "if",
  "int",      "long",     "register", "return",
  "short",    "signed",   "sizeof",   "static",
  "struct",   "switch",   "typedef",  "union",
  "unsigned", "void",     "volatile", "while"  };

const char* preProcInstructions[11] = {
  "define", "include", "undef", "ifdef",
  "ifndef", "if",      "else",  "elif",
  "endif",  "error",   "pragma"         };

/*
 * ||===========||
 * || Functions ||
 * ||===========||
*/

//Internal function for skipping whitespace. Returns new `i` value to continue from.
static long skipWhiteSpace(const char* line, long i){
  while(line[i] == ' ') i++;
  return i;
}

//Actual meat -- the readline() function.
/*
 * NOTE: At this stage of development, this function may contain data-structures and variables that do not exist yet!
*/
struct InterpreterResults readLine(const char* line){
  enum InterpreterFlags flags;
  enum ScanningFlags scan;
  struct InterpreterResults results;
  long i = skipWhiteSpace(line, i), j = 0;
  char tmp[999];
  
  //Check if preprocessor code
  if(line[i] == '#'){
    flags = preProcessorCode;
    i     = skipWhiteSpace(line, i+1);
  }
 
  for(i=i; i<sizeof(line); i++){
    if( line[i] == ' ' ) break;
    switch(line[i]){
      case '(':
    if( j == 999 ) //TODO: error
    
    tmp[j]=line[i]; j++;
  }
  
  if( flags 
    
