![Stent Logo](/doc/logo.png)

Stent
=====
Completely avoid dangling pointers in C.

How It Works
------------
By borrowing concepts from std::weak_ptr in C++ (or more closely
wxWeakRef from wxWidgets), Memory errors in C can appear in a more
deterministic manner without severely impacting the design of program
code.

Rather than using a raw pointer, the developer uses a pointer wrapper
which is guaranteed to return NULL as soon as the original data
being pointed to is freed. Rather than return NULL, Stent instead
displays a useful debug error and aborts the program, letting the
developer know that there is an issue with the code.

**Note:** Stent is intended to be enabled only during debugging.
For release builds, it can be disabled and the program will incur
no additional costs to the performance at runtime.

Getting Started
---------------
There are a few considerations that a developer needs to make before
using Stent. The following instructions should provide a brief overview
of the steps required. However, a quick look through the *src/tests/ref.c*
program should also explain these concepts.

Include the "stent.h" header file.

    #define STENT_IMPLEMENTATION
    #include <stent.h>

**Note:** Exactly one compilation unit must define *STENT_IMPLEMENTATION*
before including the header in order for the implementation of Stent
to be compiled correctly as a header-only library.

Declare a variable to store the pointer:

    struct SomeStruct *someStruct = NULL;  // Standard C
    ref(SomeStruct) someStruct = NULL;     // Using Stent

To dereference a pointer:

    someStruct->someData = 6;    // Standard C
    _(someStruct).someData = 6;  // Using Stent

To allocate dynamic memory:

    someStruct = calloc(1, sizeof(*someStruct));  // Standard C
    someStruct = allocate(SomeStruct);            // Using Stent

To free dynamic memory:

    free(someStruct);     // Standard C
    release(someStruct);  // Using Stent

**Note:** When using Stent, any time you subsequently dereference
the pointer using `_(ptr)`, it will abort since the memory pointed
to is no longer valid. This means that without checking, the program
will reliably crash in the debug build.

Forward declaring a structure:

    struct SomeStruct;  // Same for both Standard C and Stent

Defining a structure containing a pointer:

    struct Test
    {
      struct SomeStruct *someStruct;  // Standard C
      ref(SomeStruct) someStruct;     // Using Stent
    };

Pass pointer into function (Function prototypes):

    void SomeStructDoSomething(struct SomeStruct *someStruct);  // Standard C
    void SomeStructDoDomething(ref(SomeStruct) someStruct);     // Using Stent

Pass pointer into function (Calling function):

    SomeStructDoSomething(someStruct);  // Same for both Standard C and Stent

Leak Detector
-------------
There is also basic support for leak detection. Have a look at the
*src/tests/leak.c* program for more information.

For a simple example, if a leak exists, a message will appear such as:

    ***** Memory Leak *****
    Type: struct Test
    File: example/leak/main.c
    Line: 14

This is possible because Stent creates an "atexit" hook that upon
program termination, will scan internal memory for a list of data
yet to be freed.

