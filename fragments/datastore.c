// >Terminal_ / 2Term - dual "shell" and language interpreter
// Copyright (C)2019 Intense Battle 2
// This program is licensed under the GNU General Public License. Please see the beginning comments of this program's main.c file for more information.

/* About this file: datastore.c
 *
 * This file defines all the variables, pointers,
 * arrays, structures, enumerations, and unions
 * used for storing generated data during runtime
 * related to C interpretation. This ranges
 * from each's building blocks to the final
 * container.
 *
 * This code does NOT populate the data it creates.
*/

/*
 * -------------------
 * | Building blocks |
 * -------------------
*/

// Data type - Indicates attached type-keywords
// For: Variable
enum DataType {
  typeAuto, typeSigned = 0,
  typeConst,
  typeUnsigned,
  typeStatic  = 4,
  typePointer = 8,
  typeLong    = 16,
  typeChar    = 32,
  typeShort   = 64,
  typeInt     = 128,
  typeDLong   = 256,
  typeFloat   = 512,
  typeDouble  = 1024,
  typeVoid    = 2048
};

// Content - Single address to grab data as any type
// For: Variable
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
           float       sfloat;
           double      sdouble;
           long double sldouble;
};

/*
 * -------------
 * | Main data |
 * -------------
*/

// Variable - Any variable or pointer.
// For: Macro, Instruction
struct Variable {
  char*         name = calloc(1);
  enum DataType type = typeVoid;
  union Content value;
};

// Macro - Same as preprocessor macros. Can be macro functions too
struct Macro {
  char*            name = calloc(1);
  unsigned int     argc = 0;
  struct Variable* args;
  char**           content;
};

// Instruction - See dev-notes/C-interpreter/Instruction.md
// For: InstructionBlock(?)
struct Instruction {
  int*             instruction = calloc(4);
  int*             block       = calloc(4);
  int*             variable    = calloc(4);
  int*             macro       = calloc(4);
  struct Variable* internal;
  char**           action      = calloc(sizeof(char*));
};


// The following is a comment carried over from readline.c:


/*
 * NOTE:  This struct (`struct Instruction`) will have to change to fit the "math-centric" changes. Mainly, everything must more-or-less default to `performMath`, meaning a removal of this flag in favor for a
 *        "non-math" counterpart. Among this, instructions must be able to call other instructions, as sometimes a mathematical equation (especially in conditionals and the like) may require the result of another
 *        instruction.
 *
 *        On top of these, a few glaring issues have been noticed regarding instructions. The first of which is the most important and must be fixed before proceeding with any other parts of >Terminal_. This issue
 *        is that instructions cannot call native functions. After all, some of the most important functions are ones such as `malloc()`, `realloc()`, `free()`, `syscall()`, et al. Without these, much of C would break.
 *        One thing to note is that this may not mean any sturctural changes to `Instruction` specifically for these calls. After all, being able to seamlessly connect native code to non-native code is preferred. For
 *        example, if an instruction can use math to call a function pointer that points to the address of one of these functions, then that would work out perfectly. However, that would most likely mean a change with
 *        variables.
 *
 *        The other issue, which should be addressed before proceeding with any other parts of >Terminal_, is the fact that code blocks (ie, code surrounded in { ... }) cannot be easily created. The main issue that makes
 *        this a bit harder to deal with is the fact that not only can code blocks be used with `for`, `if`, and similar statements, but they can also be used completely by themselves! while any cases of `for`, `while`, etc
 *        can be grouped as a function with names like `for1`, `while3`, `if3else`, etc, blank code blocks wouldn't be able to use these well unless there's a similar setup, such as `block4` or similar. Even then, these blocks
 *        would then require a return type (which could be void), possible arguments (which could also be voided), etc
 *
 *        One method that could get rid of both problems has been devised, but currently it is unknown whether or not this course of action will be taken. To start, a new structure, `struct CodeBlock`, would have to be
 *        created. These would be similar to `Function` structures as they could contain a return type, however, it won't take arguments or have a name. Instead, `Function` will be merged with `Variable`, the difference
 *        between the two decided by if the member for the code block is empty or not. If it is, then it's a variable. If not, then it's a function. On top of this, the structure `Conditional` will be created, which will have
 *        mathematical equations has conditions (provided by an array) and an array of code blocks that matches up with the possible conditions. There may be up to 99 conditions including `else` / `default`, thus up to 99 code
 *        blocks as well. Macros will also be fit to match the new `Variable` structure. Loops will be considered code blocks with extra instructions at the start/end depending on what type of loop (generally, these will be
 *        if statements or math, such as variable assignemnts). Code blocks, despite being multiple instructions, can be put in an instruction slot as one instruction. They will contain there own scope as well, meaning they
 *        will keep track of variables created in them and destroy them when exiting unless the variables are registered as `static`.
*/