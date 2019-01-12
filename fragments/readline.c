//readline.c - implements readline()
//Copyright (C)2018-2019 Intense Battle 2
/*
 * Insert GNU GPL text here -- THIS MUST BE DONE NEXT
*/

//These vars store data for readLine(). They're not local static vars because we want other functions to see them.

/*First, since it's needed to build values, the flags. These are used to define the type of instruction
 * parsed by the interpreter. These dictate how the numbers in InstructionIdentifiers are chosen. The
 * flags are defined with ambiguity to allow them to fit the descriptions of a large range of different
 * instruction types. 
 * 
 * Tatoeba, a func call is standard code, instructs, deals with existance (grabs an existing resource),
 * is an operation, is one piece, has args (for >Terminal_, the possibility is enough), contains a resource
 * (it references a resource name), and is complete in its normal form. So, 0x00 is a
 * func call.
 * 
 * Some specific data here for dictating which flags are used in specific cases:
 *  creates data    - instruction of existance
 *  destroys data   - manipulation of existance
 *  loads/&c data   - instruction of value, references, singular
 *  edits data      - manipulation of value
 *  deals w/ funcs  - has arguments
 *  declaration     - singular, incomplete, standalone
 *  
 *  if more actions are required, it is incomplete. Tatoeba, declarations are incomplete because even though they make the
 *   variable, it doesn't have any data to give it, so it needs to be followed up with a definition. 
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
  typePOINTER  = 8,
  typeLong     = 16,
  typeChar     = 32,
  typeShort    = 64,
  typeInt      = 128,
  typeDLong    = 256,
  typeFloat    = 512,
  typeDouble   = 1024,
  typeVoid     = 2048
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

struct Variable {
  char name[999] = "nul",
  enum DataType type = typeVoid,
  struct Content value
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
