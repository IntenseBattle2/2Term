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
enum InterpreterFlags {
  standardCode = 0, preprocessorCode = 1,
  loadRefEtc   = 0, storeCreateEtc   = 2,
  variable     = 0, function         = 4,
  asOperation  = 0, asCondition      = 8,
  
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
  //std, create, variable, asOp
  declareData         = 0x07, //00000111
  //std0, inst0, exist0, asOp0, sing0, hasArgs0, alone1, incom1
  declareFunction     = 0x03, //00000011
  //std0, manip1, value1, asOp0, sing0, noArgs1, alone1, 
  //forced: edits data (manip, value, sing, ref), 
  defineData          = 0x0D, //00101100
  defineFunction      = 0x08, //00001000
  includeFile         = 0x85, //10000101
  createMacroVariable = 0x8D, //10001101
  createMacroFunction = 0x89, //10001001
  deleteMacroVariable = 0xC5, //11000101
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
struct InterpreterResults readLine(const char* line){
  struct InterpreterResults res;
  enum InterpreterFlags flags;
  long i,j;
  char tmp[999];
  //Get rid of trailing whitespace
  while(line[i]==" ") i++;
  if(line[i]=="#")    flags = preprocessorCode;
  
  for(i=i; i<sizeof(line); i++){
    if(line[i]==0x0A || line[i]==0x0D){//CR|LF test
      //end of line, finalize
      if(flags==0 && res.name==""){
        //No data collected; empty string
        strcpy(res.name, "null");
        return res;
      }
      /*
       * TODO: Add remaining finalization code
      */
    }
    
