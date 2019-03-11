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

/*
 * -------------------
 * | Data Structures |
 * -------------------
*/

struct Variable* result;

/*
 * -------------
 * | Functions |
 * -------------
*/

//Turns number from text into int
int stringToNumber(char* string, double startLocation, int* retvar)
{
  int tnum=0, i=(int)startLocation;
  while(1)
  {
    switch(string[i]){
      case '0': break;
      case '1': tnum += 1; break;
      case '2': tnum += 2; break;
      case '3': tnum += 3; break;
      case '4': tnum += 4; break;
      case '5': tnum += 5; break;
      case '6': tnum += 6; break;
      case '7': tnum += 7; break;
      case '8': tnum += 8; break;
      case '9': tnum += 9; break;
      default:
        *retvar = i;
        return tnum;
      break;
    }
    tnum *= 10;
  }
}

//Get value for runInstruction()'s inum
//NOTE: startpoint should be RIGHT AFTER IDENTIFIER.
//      This should include the possible ':'
int getInum(char* action, int startPoint, int* retvar, struct Instruction given_instruction)
{
  int i=startPoint, tnum, inum;
  
  if (action[i]!=':')
  {
    inum = stringToNumber(action, i, retvar);
    return inum;
  }
  //Get the number first
  tnum = stringToNumber(action, i+2, retvar);
  switch(action[i+1]){
    case 'i': inum = given_instruction.instruction[tnum]; break;
    case 'b': inum = given_instruction.block[tnum];       break;
    case 'v': inum = given_instruction.variable[tnum];    break;
    case 'm': inum = given_instruction.macro[tnum];       break;
    case 'd': /* TODO: Error: data is raw, not inum */    break;
    case 'r': inum = (int)typecast(128, result[tnum], 0); break; //Funny how that manages to fit :P
    case 'a': /* TODO: Error: action is text, not inum */ break;
    default:
      //TODO: Error: Syntax error - invalid identifier
    break;
  }
  *retvar += 1; //Avoid ':'
  return inum;
}

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
  // Check if given_instruction is null
  if ( given_instruction.instruction[0] == 0 &&
       given_instruction.block[0]       == 0 &&
	   given_instruction.variable[0]    == 0 &&
	   given_instruction.macro[0]       == 0    ) return -1; //ERR01: NULL_INSTRUCTION
  
  int i,j,c,tnum,inum;    // i,j,c: looping vars; tnum: temp number storage; inum = index number from instruction members
  char* action; // Current action being viewed
  struct Variable* numbers; //For getting data to be used in math
  enum {
    none,                         //Placeholder for null
    add, sub, mul, div, mod,      //ADD, SUBtraction, MULtiplication, DIVision, MODulus
    eqt, net, lth, lte, gth, gte, //EQual To, Not Equal To, Less THan, Less Than or Equal to, Greater THan, Greater Than or Equal to
    lan, lor, not,                //Logical ANd, Logical OR, logical NOT
    ban, bor, bxo, bco, bls, brs, //Bitwise ANd, Bitwise OR, Bitwise XOr, Bitwise not (2's COmplement), Bitwise Left Shift, Bitwise Right Shift
    ass, szf, add, con,           //ASSignment, SiZeoF, ADDress, CONditional (ternary)
    ind = 100                     //INDirection -- made so that other values can be combined; indirection's unique nature makes it unable to be done by itself.
  } operator;
  enum DataType typeconv; //For knowing what to convert the type to during type conversion.
        
    
  
  // In these FOR loops, the given variables can never be -1, therefore they'll loop until a `break`
  for(c=0; c!=-1; c++)
  {
    action = given_instruction.action[c];
    /* Developer's note:
     * I'm going to make `if` '>' instead of "if" just to keep the whole "one character" thing that seems to be going on with most of the code.
     * - Colin "ColinTNMP" Breese <IB2Member01@gmail.com>
    */
    for(i=0; i!=-1; i++)
    {
      switch(action[i]){
        case 'i':
          inum = getInum(action, i+1, &i, given_instruction);
          runInstruction(instruction[given_instruction.instruction[inum]]);
        break;
        case 'b':
          inum = getInum(action, i+1, &i, given_instruction);