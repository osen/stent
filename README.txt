Stent
=====
Completely avoid dangling pointers in C.

How It Works
------------
By borrowing concepts from std::weak_ptr in C++ (or more closely wxWeakRef
from wxWidgets), Memory errors in C can appear in a more deterministic
manner without impacting the design of program code too much.

Rather than using a raw pointer, the developer uses a pointer wrapper
which is guaranteed to return NULL as soon as the original data being
pointed to is freed.

Getting Started
---------------
There are a few considerations that a developer needs to make before
using Stent. The following instructions should provide a brief overview
of the steps required. However, a quick look through the "example/game"
project should also explain these concepts.

1) Include the "stent.h" header file.

2) Declare a variable to store the pointer:

struct SomeStruct *someStruct = NULL;     // Standard C
REF(SomeStruct) someStruct = {};          // Using Stent

3) To dereference a pointer:

someStruct->someData = 6;      // Standard C
GET(someStruct)->someData;     // Using Stent

4) To allocate dynamic memory:

someStruct = calloc(1, sizeof(*someStruct));     // Standard C
someStruct = CALLOC(SomeStruct);                 // Using Stent

5) To free dynamic memory:

free(someStruct);     // Standard C
FREE(someStruct);     // Using Stent

Note that when using Stent, any time you dereference a pointer using
GET(), it will return NULL since the memory pointed to is no longer valid.
This means that without checking, the program will reliably crash.

6) Forward declaring a structure:

struct SomeStruct;     // Standard C
REF(SomeStruct);       // Using Stent

7) Defining a structure containing a pointer:

struct Test
{
  struct SomeStruct *someStruct;     // Standard C
  REF(SomeStruct) someStruct;        // Using Stent
};

8) Pass pointer into function (Function prototypes):

void SomeStructDoSomething(struct SomeStruct *someStruct);     // Standard C
void SomeStructDoDomething(REF(SomeStruct) someStruct);        // Using Stent

9) Pass pointer into function (Calling function):

SomeStructDoSomething(someStruct);     // Same for both Standard C and Stent

Limitations
-----------
Pointer wrappers need to be fully declared for their size to be known
if they are to be used. However, importantly they do not need to be
fully declared withing declarations of functions using them, only
function definitions. This can be done with the following (i.e in the
SomeStruct.h file).

DECLARE(SomeStruct);

This does mean that if a structure is declared within a header file and
contains Stent references, it will require the developer to include
the header files for the types which are used. This may cause cyclic
dependencies if not handled carefully. The best solution to avoid all
this currently seems to be all struct definitions to be done within
the C file. Again, remember that the full implementation of a structure
(and thus a REF(SomeStruct)) does not need to be known within a function
declaration within a header file.

Leak Detector
-------------
There is also basic support for leak detection. Have a look at the
"example/leak" project for more information.

For a simple example, if a leak exists, a message will appear such as:

***** Memory Leak ******
Type: struct Test
File: example/leak/main.c
Line: 14

The only thing to remember is to add the following two lines to the end
of the program.

RefStats();       // Scan internal memory for list of data yet to be freed.
RefCleanup();     // Free and reset internal memory used by Stent.
