**Note: this is *not* documentation. This document is for the main developers to keep track of their thoughts and remember what the heck they were thinking when they screwed up :)**

`struct Instruction` is the structure used to store a completely compiled instruction of C code.

An Instruction (as we define it for >Terminal\_) is any kind of *statement.* For example, `foo();`, `int bar = 8 + (4 - 3);`, or even `if ( bar == 9 ) { foo(); }` are all Instructions.

An instruction can either perform math, create/edit/destroy data, call another instruction or instruction block, or any combination of the above. For example, if we took the previous instruction examples, they would (roughly) be turned into the following:

`foo();`:
* run b0

`int bar = 8 + (4 - 3);`:
* [r0] perform `*v0("bar", 64)`
* [r1] perform `4-3`
* [r2] perform `8+r1`
* [r3] perform `v:r0:=r2`

`if ( bar == 9 ) { foo(); }`:
* [r0] perform `v0==9`
* if [r0]: run b0

...Yeah, let's first note everything in brackets. Obviously, Instructions need to access and potentially modify data stored in the program or RAM, outside the Instruction structure. These are placeholders for specific data. Every piece of data an Instruction needs to access is stored in an array. Each letter stands for the array to access: b for instruction blocks, v for variables, m for macros, r for action results, d for temporary data, etc. The number stands for which id to get from the array, ie the index number. As for an identifier missing its number and instead going to another identifier (in the case of `vr0`, it's because the second identifier is ACTING AS the identifier type of the first. It's like type casting, but for identifiers.

It is important to be aware that although the external data are each kept in arrays that could be identified with the placeholders directly, the placeholders actually refer to **INTERNAL arrays *inside the Instruction structure!*** This means that, eg, `v0` is not index 0 of the `struct Variable` array but instead index 0 of the `.variable` member of the Instruction structure.

The list of identifiers are below:
* i = external **I**nstruction
* b = Instruction **b**lock
* v = **v**ariable
* p = **p**ointer
* m = **m**acro
* d = built-in **d**ata
* r = action **r**esult
* a = **a**ction

Note that i and b call `runInstruction()` and `runBlock()` respectively, a signifies essentially a GOTO, and that the built-in data is stored in an array of Variables.

The arrays they correspond to are below (`egi` is used for an example Instruction struct):
* i = `egi.instruction[]`
* b = `egi.block[]`
* v = `egi.variable[]`
* p = `egi.pointer[]`
* m = `macro[]` (global)
* d = `egi.internal[]`
* r = `result[]` (in `runInstruction()`)
* a = `egi.action[]`

Remember that all of these (except `egi.internal[]`) are `int`s containing the index of the desired content. For example, if the Instruction wanted to manipulate a variable contained in the global variable array at, eg, `variable[42]`, and that's identified as v3, then `egi.variable[3] == 42 ∵ variable[egi.variable[3]] == variable[42]`

Does your head hurt yet?

Instructions use the `.action[]` member to get the required math to perform. This member is an array of strings, traversed in order until `"END"` is reached, complete with a `NUL`. These are structured in C's natural order for math with each containing the least number of operators possible. C's natural order for math is PUMASREBAXOLAOTAC:
* Postfix: `a() a[] a->b a.b a++ a--`
* Unary^^rtl^^: `!a ~a ++a --a (TYPE)a *a &a sizeof(a)`
* Multiplicative: `a*b a/b a%b`
* Additive: `a+b a-b`
* Shift: `a<<b a>>b`
* Relational: `a<b a<=b a>b a>=b`
* Equality: `a==b a!=b`
* Bitwise And, Xor, then Or: `a&b a^b a|b`
* Logical And then Or: `a&&b a||b`
* Ternary^^rtl^^: `a?b:c`
* Assignment^^rtl^^: `a=b a+=b a-=b a*=b a/=b a%=b a<<=b a>>=b a&=b a^=b a|=b`
* Comma: `,`

... so if all of these were used, the first action would be the leftmost of a postfix and the last would be the comma.

Additional notes:
* Arguments for arrays are stored in `.internal[]` as pointers to Variable arrays with index 0 being the number of elements.  
Yes, that means arguments are stored as a pointer Variable inside of an array of Variables pointing to an array of Variables completely seperate from the array of Variables the pointer Variable is in. 

***FINALLY:*** if we revisit the 3 examples from the beginning, we now have:

### Example 1
`foo();`
* run b0, null arguments

OR

* run block[egi.block[0]], arguments [] (stored in d0)


FINALLY

`runBlock(egi.block[0], &egi.internal[0]);`

.block = [??]  
.internal = [[0]]  
.action = ["b0(d0)","END\\0"]  
result[0] = `runBlock(egi.block[0], &egi.internal[0]);`

### Example 2
`int bar = 8 + (4 - 3);`
* [r0] perform `*v0("bar",64);`
* [r1] perform `4-3`
* [r2] perform `8+r1`
* [r3] perform `v:r0:=r2`

OR

* [r0] `runFunction(egi.variable[0], &egi.internal[0]);`
* [r1] `4-3`
* [r2] `8+r1`
* [r3] `vr0=r2`

FINALLY

```
result[0] = runFuction(egi.variable[0], &egi.internal[0]);
result[1] = varifyN(64, 4-3);
result[2] = varifyN(64, 8+result[1].sint);
variable[result[0]].sint = result[2].sint;
```

.internal = [[2,"bar",64]]  
.variable = [??]  
.action = ["*v0(b0)","4-3","8+r1","vr0=r2","END\\0"]
result = [??,1,9]

### Example 3
`if ( bar == 9 ) { foo(); }`
* [r0] perform `v0==9`
* [r1] if r0: run b0, null arguments

OR

* [r0] `v0==9`
* [r1] `r0?runBlock(b0,d0):;`

FINALLY

```
return[0] = varifyN(64, variable[egi.variable[0]]==9);
return[1] = (return[0])?runBlock(egi.block[0], &egi.internal[0]):varifyN(64,0);
```

.block = [??]  
.internal = [[0]]  
.variable = [??]  
.action = ["v0==9","r0?b0(d0):0","END\\0"]