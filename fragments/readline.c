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

/*
 * MAIN DEVELOPMENT NOTE:
 *
 * Currently, the most struggle is with being able to
 * differentiate different types of instructions with
 * a single struct. During thinking about the
 * similarities between them, I remembered the one
 * thing that should be common sense and completely
 * obvious, at least to me; the entire time I've been
 * talking about how (at least in C's case) everything
 * boils down to math. Even in the eyes of the general
 * public, programming is seen as (more-or-less) math
 * with words. This is totally the case.
 *
 * At the next earliest oppurtunity to edit the
 * instruction-based structures, change these to be
 * (almost) entirely math. Even function calls are
 * math, and should be treated as such.
 *
 * Instructions that can't really be treated as math
 * (at least not easily), ie data declarations, 
 * pre-processor instructions, and flow control 
 * (for, while, if, etc), can be treated differently.
 * Because of these cases (and the fact that one
 * instruction can do more than one type of action)
 * the current structure works fine, it just needs to
 * be tweaked to be math-central and properly mark what
 * actions need to be carried out instead of trying to
 * categorize equations.
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

struct Macro {
  char name[999],
  struct Variable args[999] //999 max args. NOTE: default struct setup, `void nul`, means "not a function macro" AS LONG AS args[0] IS THIS
  char value[999,999] //[line,char]; 999 lines of 999 chars
};

//For various data types. Basically any of the keywords that can be used on variables, functions, structs, etc
enum DataType {
  typeAuto, typeSigned = 0,
  typeConst,
  typeUnsigned = 2,
  typeStatic   = 4,
  typeLong     = 8,
  typeChar     = 16,
  typeShort    = 32,
  typeInt      = 64,
  typeDLong    = 128,
  typeFloat    = 256,
  typeDouble   = 512,
  typeVoid     = 1024
};

//For variable content. Encapsulates all common types. Arrays are created as arrays of Content unions.
union Content {
  signed char schar,
  unsigned char uchar,
  signed short sshort,
  unsigned short ushort,
  signed int sint,
  unsigned int uint,
  signed long slong,
  unsigned long ulong,
  signed long long sdlong,
  unsigned long long udlong,
  signed float sfloat,
  unsigned float ufloat,
  signed long float slfloat,
  unsigned long float ulfloat,
  signed double sdouble,
  unsigned double udouble,
  signed long double sldouble,
  unsigned long double uldouble
};

//For pointers. Pointer form of Content union.
union Location {
  signed char* schar,
  unsigned char* uchar,
  signed short* sshort,
  unsigned short* ushort,
  signed int* sint,
  unsigned int* uint,
  signed long* slong,
  unsigned long* ulong,
  signed long long* sdlong,
  unsigned long long* udlong,
  signed float* sfloat,
  unsigned float* ufloat,
  signed long float* slfloat,
  unsigned long float* ulfloat,
  signed double* sdouble,
  unsigned double* udouble,
  signed long double* sldouble,
  unsigned long double* uldouble
};

struct Variable {
  char name[999] = "nul",
  enum DataType type = typeVoid,
  struct Content value
};

/*
 * TODO: look into if performing a `struct Variable*`
 *       would do the same thing. Also check for hex.
*/
//Pointer form of Variable structure
struct Pointer {
  char name[999] = "nul",
  enum DataType type = typeVoid,
  struct Location target
};

//TODO: make struct Instruction
struct Instruction {
  enum {
    CPP_Code       = 1,
    dataIsFunction = 2,
    createData     = 4,
    deleteData     = 8,
    editData       = 16,
    checkData      = 32,
    performMath    = 64,
//    conditional    = 128
  } InstructionFlags,
  struct Variable involvedVars[999],
  struct Function involvedFuncs[999],
  struct Macro    involvedMacros[999],
  char involvedMath[999],
  short involvedKeywords[999]
};


//TODO: make struct Function
//      Note: Structure ideally should be like struct Macro, but with an array of Instruction structures. These are where flags come into play.
struct Function {
  char name[999],
  enum DataType returnValue,
  struct Variable args[999],
  struct Instruction code[999]
};


/*
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!                                                          !!
 * !!  VERY IMPORTANT NOTE:                                    !!
 * !!                                                          !!
 * !!  THE BELOW STRUCTURES, ENUMERATIONS, ETC ARE /USELESS/.  !!
 * !!  A NEW STRUCTURE "Instruction" WILL BE CREATED!!!!!      !!
 * !!  NONE OF THE BELOW WILL BE USED!!!!                      !!
 * !!  ALL OF IT MUST BE SCRAPPED!!!!!!!                       !!
 * !!                                                          !!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
*/

enum InterpreterFlags {
  standardCode = 0, preprocessorCode = 1,
  loadRefEtc   = 0, storeCreateEtc   = 2,
  variable     = 0, function         = 4,
  
  /*
   * Currently changing flags; current setup doesn't work at all. Current leftovers:
  
  singlePiece  = 0, multiPiece       = 16,
  hasArgs      = 0, noArgs           = 32,
  reference    = 0, standAlone       = 64,
  complete     = 0, incomplete       = 128
  
  *Extra note: flags are (very loosely) based off how assembly code works
*/
};

//Then, the flag results
enum InstructionIdentifiers {
  /*
   * TODO: Sort alphabetically, then mark as such
   * AND: Fix all of these to the new flag system
  */
  //std, ref, func, asOp
  callFunction        = 0x00, //00000000
  //std, create, variable
  declareData         = 0x07, //00000111
  //std, create, function
  declareFunction     = 0x03, //00000011
  //std, store, variable
  defineData          = 0x0D, //00101100
  //std, store, function
  defineFunction      = 0x08, //00001000
  //pre, ref, func
  includeFile         = 0x85, //10000101
  //pre, create, variable
  createMacro = 0x8D, //10001101
  deleteMacro = 0xC5, //11000101
  deleteMacroFunction = 0xC1, //11000001
  /*
   * TODO: Complete list of instruction identifiers
  */
};

//Finally, the package!
struct InterpreterResults {
  enum InstructionIdentifiers type;
  char name[999];
  char modifiers_1; //too generic… maybe another enum?
  /*
   * TODO: Fill according to needed data
   *       KEEP AMBIGUOUS
 */
 
//Internal var for skipping whitespace
static long skipWhiteSpace(const char* line, long i){
  while(line[i] == ' ') i++;
  return i;
}

struct InterpreterResults readLine(const char* line){
  enum InterpreterFlags flags;
  enum ScanningFlags scan;
  long i = skipWhiteSpace(line, i), j = i;
  char tmp[999];
 
  //Check if preprocessor code
  if(line[i] == '#'){
    flags = preProcessorCode;
    i     = skipWhiteSpace(line, i+1);
  }
 
  for(i=i; i<sizeof(line); i++){
    if
