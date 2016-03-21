stent
=====
Avoid dangling pointers in C.

How It Works
------------
By borrowing the idea of a std::weak_ptr in C++ (or more closely wxWeakRef
from wxWidget), C can also work in a more deterministic manner without
impacting the design of a program code too much.

Rather than using raw pointer, the developer uses a pointer wrapper
which is guarenteed to return NULL as soon as the original data being
pointed to is freed.

Getting Started
---------------
There are a few considerations that a developer needs to know before
using stent. The following instructions are a brief overview of the
steps required.

1) Include the "stent.h" header file.

2) Instead of using the following to declare a pointer:

struct SomeStruct *someStruct = NULL;

use

REF(SomeStruct) someStruct = {};

3) Instead of using the following to dereference a pointer:

someStruct->someData = 6;

use

GET(someStruct)->someData;

