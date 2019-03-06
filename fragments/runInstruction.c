// >Terminal_ / 2Term - dual "shell" and language interpreter
// Copyright (C)2019 Intense Battle 2
// This code file is licensed under the GNU GPL. Please read the begiining comments of this program's main.c file for more information.
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
 * instructions; instead, it queries the preprocessor
 * function for these tasks should it ever be handed
 * such data, though the pre-processor should have
 * already handled such requests during the processing
 * of code lines into Instruction structs.
 *
 * Here, it should be stated what an instruction counts
 * as. In C, we think of an instruction as any
 * statement that either declares or defines data,
 * calls a function, or uses any of the given keywords.
 * These may or may not end in a semicolon, and may or
 * may not perform math. They also may or may not have
 * code blocks (ie, instructions wrapped in braces)
 * attached to them. Some examples of single
 * instructions are below:
 *
 * `num = 4 * num2 + (num < num2);`
 *
 * `someFunction();`
 *
 * ```
 * if( num == 28 )
 * {
 *   num3 = anotherFunction(num2);
 * }
 * ```
 *
 * Each of these instructions are different. One of
 * them even contains other instructions. However, they
 * all count as one, more or less. It should be noted
 * that some resolve to calling other instructions, and
 * so could resolve to multiple in that sense. However,
 * they all resolve to variable amounts of steps. For
 * example, here are the steps broken down from the
 * previous instructions:
 *
 *
 * `num = 4 * num2 + (num < num2);`
 *   check if var 'num' is less than var 'num2' [r0]
 *   multiply 4 by var 'num2' [r1]
 *   add result 'r0' to result 'r1' [r3]
 *   assign result 'r3' to var 'num'
 *
 * `someFunction();`
 *   retrieve instruction list tied to 'someFunction'
 *     and run with argument list '[]'
 *
 * ```
 * if( num == 28 )
 * {
 *   num3 = anotherFunction(num2);
 * }
 * ```
 *   check if var 'num' equals 28 [r0]
 *   if result 'r0' resolves to true, then run
 *     instruction list below:
 *       retrieve instruction list tied to
 *         'anotherFunction' and run with argument list
 *         '[var 'num']' [r1]
 *       assign result 'r1' to var 'num3'
 *  else abort
 *
 *
 * Evidently, one instruction resolves to multiple
 * actions, though of course this is redundant as it was
 * already mentioned and demonstrated. The reason for
 * such enforcement is due to the definition of
 * "instruction" in Assembly. In the end, instructions
 * as we define them are groups of Assembly instructions
 * that would be required for a single C statement.
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
      