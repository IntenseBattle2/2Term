// >Terminal_ / 2Term - dual "shell" and language interpreter
// Copyright (C)2019 Intense Battle 2
// This code file is licensed under the GNU GPL. Please read the beginning comments of this program's main.c file for more information.
/*
 * About this file: runInstruction.c
 *
 * This file defines the required variables, structures,
 * functions, ETC unique to the main defined function
 * `runInstruction()`. This function takes Instruction
 * structs and carries out its contents. These include
 * creating/editing/destroying all types of data
 * structures, running functions, and performing math.
 *
 * `runInstruction()` does not handle pre-processor
 * instructions as these do not have a stored state and
 * should have already been handled.
 *
 * For information on Instruction structures, please
 * see dev-notes/C-interpreter/Instruction.md
*/

/*
 * This file relies on the following other files and
 * their implemented data:
 *
 * readline.c
 * |struct Instruction
 * ||...
*/


//The main function: runInstruction()
//  NOTE: In this stage of development, this function may contain data that is not yet defined.
/* Returned int gives status number:
 * -2 = *ERR* Data to be created already exists
 * -1 = *ERR* Instruction is null
 *  0 = Successful run, nothing to report
 *  1 = *WARN* Instruction run by pre-processor
*/
int runInstruction(struct Instruction given_instruction)
{
  if ( /*given_instruction == void version*/) return -1;
  if ( given_instruction.preproc != -1 )
  {
    runPreProc(given_instruction);
    return -2;
  }
  switch (given_instruction.keyword[0]) { //detect data declaration
    case 1:  //auto
    case 4:  //char
    case 5:  //const
    case 9:  //double
    case 11: //enum
    case 12: //extern
    case 13: //float
    case 17: //int
    case 18: //long
    case 21: //short
    case 22: //signed
    case 24: //static
    case 25: //struct
    case 28: //union
    case 29: //unsigned
    case 30: //void
    case 31: //volatile
      //see if var already exists
      if ( getVarByName(given_instruction.variable[0]) ) return -2;
      