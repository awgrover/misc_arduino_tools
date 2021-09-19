#pragma once

#include <Streaming.h> // Streaming.h - supporting the << streaming operator, Mikal Hart

// why is this missing?
inline Print &operator <<(Print &obj, const __FlashStringHelper* arg) { obj.print(arg); return obj; }

// NB: do not attempt to use Serial in static initializers (at toplevel scope)
// because Serial may not have been init'd yet! And you can't force it.
// "C++ Static initialization order fiasco"
