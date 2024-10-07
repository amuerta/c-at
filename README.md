# C-at virtual machine - SETVM

Virtual machine is WIP and isn't meant to be used on anything more than just messing around.

## TODO
+ more stack interation instructions: 
    - iload, fload, sload, bload, lload ... save -> load and save of different sizes
    - icpy,  fcpy, -> copy data from register to register
    - skip  -> skip N instructions if first arg is >= 0
    - ipop, spop, bpop, spush, fpush ... -> push and pop instructions of all sorts
+ stack allocation: allocstack, heap allocation: allocheap 
+ binary format header with supported major vm version, 
+ consider reworking mechanism of stack?

## ROADMAP
+   x86-64 features:
    + add 64bit registers that are 64bit wide or made of 2 32bit one.
    + make vm decide at start time what way to manage 64bit registers
    + in either case there should be 256 (64 * 4) bit of register space meant for 
            big variable types f64 or i64
+   Dynamic C libraries linking
    - vm can link external C libraries using the header provided path's 
    - make both quick FFI and binding methods to link with C
    - FFI allows to Just call functions with arguments and return size of 2kb (2048 bytes)
+   more real instructions:
    - parital load of register values
    - floating point context comparisons
    - integer context comparisons
    - pointer dereference operation
    - string support, index retrival
+   heap space:
    - make vm allocate memory from os and store pointers to it on
            the "Heap space"
    - heap space tracks of pointers trough program live span and complains
            if not all memory if freed in the end, does it automatically then.
    - heap space instuctions
+   implement stack:
    - stack manipulation instructions: [ push, pop, load, save ] **DONE**
    - function call stack that stores only about call chain WIP
    - stack variables and system to track when the space should be reserved 
            and when the stack pointer has to offset to free space for new data. (closely related to function callstack)
+   assembler features:
    - binary operation evaluation (in order to have convinient address offsets)
    - entry point, data section before it, and header for telling what libraries and functions to link
    - character evaluation
    - string evaulation
    - cli arguments
+   Iterative improvement:
    - ~~change in `basic_assembler.c` all hacky string manipulations to more readable header lib `str.h` versions.~~
    - rewrite `basic_assembler.c` to complete implementation
    - support for more platforms: Windows, Android
    - make language utilize 64 bit data registers on 64bit platforms, or operate on two 32bit registers
    - make runtime library for the VM: print functions, OS interactions, Etc.
+   Fututre goals:
    - concurent execution of code
    - easy build-in way of creating threads and running pices of the program on them.

## TOOLSET

### C-At Virtual Machine - a pun name to instruction: SET
+ takes `.caic` files and runs 16 byte instructions in oreder.
+ header can contain dynamic C libraries and other crutial information to run the program.
+ can output internal parts of a big instuction list into other files for easier debugging
+ has mixed architecture but with bias towards utilizing registers over stack
+ stack is used to store and local vars
+ function call frame stores inforamtion about function calls: basic function pointer, argument count, etc.

### C-At Assembler - shorten: CAASM
+ simple assembler with no macros capabilites.
+ compiles to C-at instruction code `.caic` 
+ asserts and prints error 
+ does two passes :
    1. finds all labels, literalls, directives and buffers them.
    2. iterates over instructions and assembles them with taking data from buffering into consideration.
+ can disassemble compiled instuctions into human readable view. all label names and literalls are lost.
+ can debug view program stack.

### Set debugger - shorten: CADB
+ Can load parts of a instuction code into memory and run it.
+ Has repl to allow user to poke bits of compiled code.
+ Has connectivity with CAASM to perform Just-In-Time compilation and run the IC on the Machine.
+ Allows breakpoints and debug information, which are all stored in the program or separate file
    if program was compiled with C-at compiler.
+ Allows to step trough the execution process and print the current execution order.

## Example of assembly:

```
# basic recursion function call example
	set r0 0
	rec
	push r0
	call 5
	ret r0 #4


Lrec: 
	pop r0 #5
	set r1 1
	set r2 5
	add r0 r0 r1
	regdump r0
	nejmp 12 r0 r2 
Lrecend: # 11
	ret r0 
Lreccon:
	rec #12
	push r0
	call 5 #14
	ret r0
```

## NOTE:

The virtual machine is Work In Progress and is on very early stage, everything can be a subject to change, for now its no bigger than a toy, this should be kept in mind when working with it.
