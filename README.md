# Composing Arduino Code (copypasta)

(It works! Look at the examples/blink.ardmodule, built from examples/blink and examples/blink_faster)

What if we took copypasta seriously? No text based programming language 
is friendly towards copypasta, despite it being the overwhelming method
used to write code. By volume, ALL code is written with copypasta (+/-
0.00001%). Please feel free to argue about the "quality" of such code (somewhere else),
but that's not (quite) the point of this project.

Yet, graphical programming languages (actual graph style, but even scratch style) can be
very friendly for copy-paste. A pasted chunk of a graph is just a parallel thread
of execution. Assuming you got all the config/init inputs. But, even
without, you get a chunk of "code" that you can use, requiring only
some config/init inputs. 

What does that look like in a text programming language? You have to pick
apart each "sketch", to merge the library-includes, the globals, the object
initialization (which requires careful examination). You can merge the 2 setup()'s and the 2 loop()'s by just naming
them like loop1 and loop2 and calling them from the real loop() (similarly for setup).
Leaving aside the evil that is delay(), you know have 2 sketches merged. And, it probably
won't work (ha). 

Try having a beginner/less-experienced person try this with 2 Arduino
sketches: tape your mouth shut, and tie your hands behind your back, and just watch. Also,
take notes when you see something that won't work. Think about why it seemed obvious to
do a thing, but that thing won't work. Think about why the whole attempt would have
worked much better (maybe even almost "just worked") in a graphical language.

But, what about a piece of code that you want to extract? I think it is common
to want to use just a part of a sketch, and people will try to copy-paste: and that will
almost certainly cause compiler errors (or crash the program at best). Because,
you need to include the relevant library-includes, the globals/object-initialization, 
part of setup(), part of loop() and its corresponding variable declarations.

In a graphical programming environment, wiring one-thing to other code is easier: you
can find the other thing, and drag a connection across. What would
be the textual equivalent? Digging through the code to find where the
call goes, making sure the callee is in scope, figuring out how to
compose the argument list (one dragged line vs a whole call), and so on.
Which is perfectly normal for competent programmers.

Graphical programming puts all the pieces for a "node" in one place and then merges them
into the right compile and run-time locations. 

We'll treat a sketch like that:

First, the style of programming has to change a bit. Instead of monolithic programs,
we need to write the equivalent of a graphical-programming "node": inputs/outputs and
one behavior. And, then express how to wire them together. (and worry about
resource management, blah blah blah). The point being, write more (copy-paste) composable code.
Which isn't that hard or different.

The idea is to have several/many .ino's in your sketch (each a simple module), and automatically merge them.
You can just add a new .ino, and it should work.

Second, we'd change the Arduino IDE to support this and automate the merging.
It already does (hideous) magic on a sketch, would this be any worse? And,
we get all the C++ (GCC/whatever) optimizations, like inlining, etc., so maybe
it's not so inefficient.

Of course, this whole idea should be extensible to python, ruby, etc. What
are the limitations for other languages (i.e. compiled/non-IDE environments
like C/C++ & make)?

## Proof of Concept

As a proof of concept, just write "nodes" as regular .ino sketches, and then:

Wrap a sketch in `namespace`, then call each namespace's setup() and loop(). We get
library-include & global/object-initialization automatically.

It works! Look at the examples/blink.ardmodule, built from examples/blink and examples/blink_faster

Now, will wiring things together textually be manageable and copypasta friendly?

## To do

finish compile: should produce the root .ino

then how to compile in wiring

multiple copies of same module

figure out how to wire them together (declare inputs/outputs)

how to do config/in/out? can't be const can they?

break things down more into modules as per FIXME's

do the "by default do something useful" thing, until configured otherwise.

I have more principles in my little yellow notebook, but I'm tired of typing now.
