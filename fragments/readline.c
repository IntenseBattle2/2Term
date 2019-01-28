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
  char            name[999] = "NUL";
  struct Variable args[999];      //999 max args. NOTE: default struct setup, `void nul`, means "not a function macro" AS LONG AS args[0] IS THIS
  char            value[999,999]; //[line,char]; 999 lines of 999 chars
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
  signed   char        schar;
  unsigned char        uchar;
  signed   short       sshort;
  unsigned short       ushort;
  signed   int         sint;
  unsigned int         uint;
  signed   long        slong;
  unsigned long        ulong;
  signed   long long   sdlong;
  unsigned long long   udlong;
  signed   float       sfloat;
  unsigned float       ufloat;
  signed   long float  slfloat;
  unsigned long float  ulfloat;
  signed   double      sdouble;
  unsigned double      udouble;
  signed   long double sldouble;
  unsigned long double uldouble;
};

//Variable structures, for keeping track of variables for the intepreted C code.
//Using `struct Variable*` will make a pointer structure.
struct Variable {
  char            name[999] = "nul";
  enum   DataType type = typeVoid;
  struct Content  value;
};

//Contains all necessary data of a single instruction.
//An instruction is either a C command via keyword, function call, or variable/macro stuff (AKA math)
struct Instruction {
  enum {
    CPP_Code    = 1,  //CPP = C Pre-Processor
    flowControl = 2,  //Flow controls = C commands. This is the "keyword is involved" flag.
    createData  = 4,  //This instruction creates data, be it a variable, macro, function, or alike.
    deleteData  = 8,  //This instruction deletes data, be it a variable, macro, function, or alike.
    performMath = 16, //This variable performs a mathematical operation. Standard and will probably be removed in favor of a "calles another instruction" flag.
  } InstructionFlags;

  struct Variable involvedVars[999];   //For var creation
  struct Function involvedFuncs[999];  //For func creation
  struct Macro    involvedMacros[999]; //For macro creation
  char            involvedMath[999];   //For math, obviously
  short           involvedKeyword;     //Whatever the keyword is (if any, eg "if", "return", or "while"), but by its index number in keywords[]
};

//Contains all necessary data of a single function.
//A function in this case is a "package" of multiple instructions executed in succession.
struct Function {
  char               name[999] = "NUL";
  enum   DataType    returnValue = typeVoid;
  struct Variable    args[999];   //Note that if an entry == `void nul`, then that means that's the end; no more arguments; REGARDLESS of what's passed that point!
  struct Instruction code[999];   //Instructions are considered to end first instance of an empty instruction (0, [0]=='void nul', [0]=='void NUL(void)', [0]=='NUL', <empty>, 0) REGARDLESS OF WHAT'S PASSED THAT POINT!
};



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
  long i = skipWhiteSpace(line, i), j = i;
  char tmp[999];
  
  //Check if preprocessor code
  if(line[i] == '#'){
    flags = preProcessorCode;
    i     = skipWhiteSpace(line, i+1);
  }
 
  for(i=i; i<sizeof(line); i++){
    if( line[i] == ' ' )
