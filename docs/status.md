Status and Roadmap
==================

Panda is currently in *pre-beta*. It is working well enough to have implemented
a 50,000 line compiler and a number of sample applications, and for me to use
day-to-day as my primary programming language, but it is still incomplete and a
lot of hard work remains before it is a bulletproof, complete, production-ready
language.

There are a *lot* of little things that need work, but this document is only
intended to cover some of the very high-level plans.

A Note On Backwards Compatibility
=================================

Obviously, backwards compatibility is an important goal in any programming 
language -- nobody wants to find out that their existing software has
suddenly stopped working. However, while Panda is this early in development,
backwards compatibility is not yet 100% guaranteed.

Breaking changes will not be made lightly. The entire compiler is written in
Panda, after all, which means any changes are going to hit me much harder than
they will hit anyone else. But if I discover a problem area, I'd rather fix it 
now and deal with some brief pain than be stuck with a poor decision for the 
rest of Panda's lifespan.

Once Panda hits actual beta, every piece of Panda will be marked with its
expected stability (one of STABLE, UNSTABLE, EXPERIMENTAL, or DEPRECATED), which
you will see clearly noted in the documentation. Changes to STABLE components 
are not expected to happen, though that is not a hard guarantee yet. UNSTABLE 
components *probably* won't change, and all bets are off on EXPERIMENTAL 
components. I reserve the right to introduce new keywords during this process.

Once Panda hits 1.0, anything marked STABLE will not be changing and you will be
able to trust Panda's backwards compatibilty as much as any other language.

Definite Upcoming Changes
=========================

Performance
-----------

Performance is an important long-term goal for Panda, but early on it is more
important to worry about getting the language completely working than it is to
worry about squeezing every last ounce of speed out of it. Currently, Panda is 
probably going to be at least three or four times slower than Java for 
general-purpose code even when safety checks are disabled; certain
microbenchmarks are likely at leat two orders of magnitude slower.

While that's obviously not great, consider that Java has a twenty-year head 
start and started off *far* slower than Panda is now. Panda is going to get 
considerably faster after it is feature-complete and focus turns towards
optimization.

It's worth noting that Panda's current approach to optimization is "don't". Code
generation is about as stupid as it can possibly get, and I'm using the fact 
that the languages I'm outputting to (LLVM IR and Java) have their own 
optimizing compilers as a crutch. 

Therefore, we end up with pretty tolerable (though not awesome) code despite 
zero work on optimization. Obviously this will change, but it's not completely 
clear to me where the best optimization payoffs are, since duplicating 
optimization work which is also going to be performed by LLVM or javac will suck
up a lot of time while providing little or no payoff (possibly even negative
payoff, as our optimized code might prevent even better optimizations that LLVM
would have been doing on the naive unoptimized code). Also worth noting that 
while we're not actually optimizing yet, a lot of the basic groundwork is there:
we are building a CFG, performing (very basic) dataflow analysis, and so forth 
(though given the currently-limited applications of this data, there are 
probably some serious bugs in the existing code).

APIs
----

Panda's core libraries cover only the most basic of basics. There's no date or
time API, no networking, no arbitrary-precision math, only the most incredibly
simple of a graphics API, no sound API, and there are a million other things 
missing. This isn't because those things aren't important; it's simply because I
am currently the only person working on Panda and it's a very, very big project.

The "init hole"
---------------

Panda's current type system is unsound in at least one way. It's currently
possible to call instance methods in a constructor prior to completely 
initializing the object. This means that it's possible to "see" fields of the
object that haven't actually been initialized yet, and thus "see" non-nullable
fields with a `null` value, generally resulting in a crash. I refer to this as
the "init hole".

In practice it isn't a *huge* deal -- in writing the entire compiler and a 
bunch of other Panda programs, I have of course made tons and tons of mistakes
and gotten thousands of errors at both compilation and runtime, but I have never
once run into the init hole in real code. But it stresses me out that the 
problem exists, so I will most likely fix it before 1.0. Unfortunately, the fix
may be a bit draconian. The only truly safe way to fix it is to disallow any 
calls to instance methods which could potentially access an uninitialized 
non-nullable field. In the case of private fields, we can generally make that
determination pretty well. In the case of protected / public fields, we'll have
to end up disallowing any calls (even indirect calls) to methods which could be
overridden, which in most cases will boil down to "you can't call any instance
methods before initializing all of your public / protected fields". I'm honestly
not sure yet whether the disease is worse than the cure.

Threading
---------

I *think* I'm happy with the overall thread model (communication via messages,
no shared mutable state). It definitely imposes some additional costs relative
to the good old fashioned "everything can access everything, deadlocks and 
unexpectedly broken data for everyone!" model, but I'm hoping once the APIs 
are fully fleshed out the safety and comprehensibility benefits will outweigh
the costs. Frankly, I'm still not quite 100% sure this will be the case; it's
possible I'll end up saying "screw it" and reintroduce class variables and all
that (Panda had them, once upon a time; some of the machinery is still present
in the compiler).

Assuming I stick with the current approach, there are still a lot of things that 
need work:

1. The current implementation of the threading system sucks. It was a very
    quick-and-dirty implementation to begin with, and then I found out that some
    of the things I was doing didn't play right with this particular garbage 
    collector, and then I found some other problems, and... well, you know how 
    it goes. It's a bit of a mess (ok, a huge mess) right now.
2. The API needs work. Creating new threads feels "heavier" than I would like it
    to; might be worth introducing a keyword or at least some kind of simpler
    syntax to spawn a new thread. There's also no way to select among multiple 
    MessageQueues, and that is *desperately* needed.
3. Threads are currently just operating system threads, which are very 
    heavyweight. I'd like to have a very low cost for threading, so we can 
    easily have tens of thousands of threads running. Might be worth considering
    multiplexing Panda threads onto the same OS thread, though of course that
    carries its own tradeoffs.
4. `Thread.context` has a lot of potential, but the current implementation isn't
    quite right. Basically, what I really want is a means of locking "global"
    variables, configuration settings, and so forth to a particular thread or
    group of threads. Obviously that isn't what we actually have at the moment,
    and even to the extent that we do have it, it's harder than it should be to
    work with. Unfortunately I feel like I'm orbiting around the right approach,
    occasionally catching glimpses of it, without ever quite solving the 
    fundamental problems.

Unicode
-------

Panda has some of the groundwork for Unicode support -- `Char` is 16 bits long, 
streams have both `Int8` and `Char` methods, etc., but no "real" Unicode support 
is actually present. Panda is currently just zero-extending bytes to make 
characters, and truncating characters to make bytes, which means it really only
works for ASCII characters. Of course I know this is not the right way to handle
things :-). It's just a temporary hack until I have time to implement proper
Unicode support, but that's a big job and we can still do a lot of interesting
things even with an ASCII-only Panda.

*Ethan's thoughts:* I still have some open questions about the best way to 
handle things. For instance, I am well aware that a 16-bit Char is not big 
enough to hold all Unicode characters; but extending it to get rid of surrogates 
doubles the storage costs and slows things down for relatively little gain. 
Furthermore, even if we get rid of surrogates by using a 32-bit char, we can't 
get rid of decomposed character sequences like LATIN SMALL LETTER A + 
COMBINING ACUTE ACCENT. If we already have to deal with the fact that character
sequences like this exist, are surrogates really that much worse? Doubling
storage costs is a *huge* deal, and it wouldn't actually fix things. How do we
handle the API issues caused by the fact that a "Char" isn't actually a "Char",
it's really just a UTF-16 code point? Is "Char" even the right name for the
type?

Of course, we could also internally store a String as something other than an
Array<Char>, but that comes with a whole host of its own problems. I really need
to perform a survey of how other languages are solving these issues. I suspect
there's really not a great solution to a lot of these issues, but I'm open to 
suggestions.

Nullable Numbers
----------------

Every non-`null` value in Panda is an object, even numbers like `12`. But for 
the sake of efficiency, Panda doesn't actually create a full-blown object 
instance unless it needs to. If you consider a statement like

    x += y

where `x` and `y` are `Int`s, this will compile to the same basic CPU math
instructions as it would under any other language; no actual objects need to
exist anywhere. Of course, when you do something like add a number to an
`Array<Object>` or call `processObject(o:Object)` with a number, Panda will end
up transparently creating an actual `Object` instance for the number, and that's
probably the best we can do there.

But there's an intermediate case, too: nullable numbers. If a function returns
`Int?`, we don't really need to create a full-blown `Object` instance for this:
we really only need one more bit of storage somewhere, so that we can 
distinguish between `null` and non-`null` numbers. Unfortunately the current
approach is simply to fall all the way back to creating actual `Object` 
instances. Even more unfortunately, there are a couple of spots where this 
actually creates major performance concerns: several common `String` operations
and byte-by-byte/char-by-char `InputStream` reading both use nullable numbers to
signal exceptional results, and I'm sure the sheer number of objects we are
creating is murder.

So by fixing nullable numbers to just be bigger (probably a struct consisting of
the number itself and an extra bit) and using that additional bit to distinguish 
`null` from non-`null`, we should be able to massively improve performance on
some common operations. On the Java side of things, we could similarly extend
the range of numbers smaller than `Int64` (using, say, a `short` to represent 
`Int8?`) to give us the extra bit we need; this would at least fix the problem
for integers 32 bits and smaller, which is the 99% use case.

Value Improvements
------------------

The `Value` class exists as a hint to the compiler that this object can be
passed by value, and it is ok to fold similar instances together into a single
object. The compiler is not currently performing either of these optimizations,
but when it does it should lead to a significant speed bost.

Memory Management
-----------------

Panda currently uses the Boehm-Demers-Weiser garbage collector to manage its
memory, and its approach to memory is... simplistic. "Just allocate everything
on the heap and let the collector sort it out" is refreshingly straightforward, 
but it's definitely not the smartest approach. Escape analysis, particularly
given Panda's approach towards functions, should allow a significant number of
allocations to end up on the stack instead of the heap. A concept of "wholly 
owned" objects (objects which are completely encapsulated inside of other 
objects and never visible to outside code) will allow objects to be merged 
together and allocated as a single unit, reducing load on the garbage collector. 

Furthermore, I have learned that despite all of the documentation claiming that 
the Boehm-Demers-Weiser garbage collector is purely a conservative collector, it 
actually *can* be used as an accurate GC! We need to switch to using
GC_gcj_malloc, which shouldn't be very difficult.

Finally, I'm not even 100% convinced that garbage collection is the right
overall approach. An early version of Panda used automatic reference counting as
its garbage collection algorithm; I've variously been tempted to go back to 
that or something else more predictable. Unfortunately, everything has its
downsides...

Better Debugging
----------------

Panda does not currently embed debug information in its executables, meaning you
can't get line numbers from its exceptions. It also does not catch stack 
overflows, meaning that they simply crash with a segfault. Obviously this needs
to get fixed, but it has been a surprisingly minor annoyance for me in practice: 
for the time being, one can simply recompile the offending program to run on the 
JVM, which does catch such errors and includes line numbers (though you have to
check the .java source to figure out where in the .panda file the error
corresponds to. That's something else that needs to get fixed!)

Reflection
----------

Reflection probably won't make it until post-1.0, but it's an obvious missing
feature.

Native use of Java objects
--------------------------

Currently, accessing Java objects from Panda requires jumping through a lot of
hoops. I'd like to simplify this, although the resulting program will obviously
then be JVM-only. Accessing Panda objects from Java will likely continue to be 
fairly difficult, but I'd still like to reduce the stupidity of how Panda 
objects are represented in Java.

JavaScript Output
-----------------

The JavaScript output engine is already almost completely functional. With just
a bit more work, you'll be able to write a game in Panda and run it in a web
browser.

Resources
---------

The compiler will be able to embed files into the executable, sort of like
embedding resource files into a Java .jar file. (Well, exactly like that, when 
you're compiling to a .jar file...)

Compile-Time Safety Errors
--------------------------

Violation of an assert, precondition or postcondition is a runtime error. When
the dataflow analysis is sufficiently finished, these will also become 
compile-time errors if the compiler can prove that a safety error always occurs.
For instance, the following code:

    def stack := new Stack<Int>()
    stack.push(1)
    stack.push(2)
    stack.pop()
    stack.pop()
    stack.pop()

should result in a compiler exception. Stack's constructor has a postcondition
of length = 0, stack.push has a postcondition that it increases the stack length
by one, and stack.pop has a precondition that length is greater than zero and
a postcondition that it reduces the stack length by one. Given these conditions,
a sufficiently-smart analyzer could easily prove that the last call to 
stack.pop() happens when the stack length is zero and therefore generate a 
compile-time error.

Obviously, such clear-cut errors won't be incredibly common, but they do happen
and it would be nice to have the compiler detect them. 

Potential Features Which Might Happen Eventually
================================================

Some of these proposed features may have huge, glaring flaws which render them
obviously stupid and/or pointless as described. I'm just writing down things I
was thinking about so I can keep track of them.

Default Interface Implementations
---------------------------------

Enterprise software has shown us the importance of interfaces and pluggable
implementations, but I'm not convinced that it has shown us the best way to
implement these things. Something I'm considering for the future of Panda is
*default interface implementations*: basically, interfaces can define 
constructors, just as they do methods, and would be guaranteed that every class
implementing that interface would provide an implementation of that constructor.
Then you would be able to define in a context what the default implementation of
a given interface is, and simply create that interface as if it were a class:

    def m := new Map<String, Object>()

and actually end up with an instance of the default implementation of that
interface. In other words, no Factory classes -- just instantiate the interfaces
directly, and you can seamlessly switch out implementations at will.

Extensions
----------

I am considering a syntax to extend existing classes, similar to Objective C's 
categories. This would be a huge win in terms of keeping the core libraries
modular; much as I like the regular expression features in `String`, for 
example, I'd prefer to see them split out into a separate module (much as 
`panda.gl` is currently broken out) so we don't absolutely have to have a 
regular expression library in every Panda installation, and the same basic deal
for every "big" and not-strictly-necessary module currently included.

Inner Classes
-------------

At some point I'll probably add inner classes, similar syntactically to Java's.
It's not as high of a priority as it was for Java, however, because Panda has 
first class functions and therefore does not have nearly the need for anonymous
inner classes. Even if it gains support for named inner classes, Panda will 
probably not offer anonymous inner classes at all: with tuples, first-class 
functions, lambdas, MessageQueues, and other powerful language features, there 
just isn't nearly as much need for them as there was in Java.

Inner enums, however, have *got* to happen prior to 1.0.

Multiple-Dimension Indexing
---------------------------

I am strongly considering extending the index and indexed-assignment operators
to accept multiple values, allowing you to write things like:

    dest[x, y] := src[x, y]

Syntactically, this is quite straightforward, and I could probably do the
implementation in an afternoon. The hesitation mainly comes from open questions
about interaction with other language features, such as the slice syntax.
Keeping slicing single-valued as it is now, forcing you to use tuples to specify
multi-dimensional slices, is a potential answer.

Proofs
------

It isn't always easy to work out why the compiler is claiming the things it's
claiming. Most of us have at some point said "What do you mean, I didn't 
initialize that variable?" while carefully tracing execution paths to find the
path we missed. To the basic uninitialized / double-initialized errors that many
languages report, Panda adds complaints about improper uses of null, mutation of
externally-visible objects, mutable vs. immutable methods, (eventually) 
compile-time safety errors... in other words, there are a lot of things the
compiler can complain about which won't necessarily be immediately obvious.

I would like to eventually add the ability to construct a *proof* of a given
situation. The basic idea (subject to change, of course) is that the output for
compiling:

    def stack := new Stack<Int>()
    stack.push(1)
    stack.push(2)
    stack.pop()
    stack.pop()
    stack.pop()

would look something like:

    error: Test.panda:6:6: precondition 'length > 0' of 'panda.core.Stack::pop()'
    is always violated. Compile with -proof for proof.

And then if you recompiled with the -proof argument, you would get output 
similar to:

    error: Test.panda:6:6: precondition 'length > 0' of 'panda.core.Stack::pop()'
    is always violated. Proof:
        1. Test.panda:1:14: 'length' must be equal to 0, by 
                'panda.core.Stack::constructor' postcondition 'length = 0'
        2. Test.panda:1:6: 'stack' is set equal to 'panda.core.Stack' value from
                step 1
        3. Test.panda:2:6: 'stack.length' must be equal to 1, by
                'panda.core.Stack::push' postcondition 'length = @pre(length) + 1'
        4. Test.panda:3:6: 'stack.length' must be equal to 2, by
                'panda.core.Stack::push' postcondition 'length = @pre(length) + 1'
        5. Test.panda:4:6: 'stack.length' must be equal to 1, by
                'panda.core.Stack::pop' postcondition 'length = @pre(length) - 1'
        6. Test.panda:5:6: 'stack.length' must be equal to 0, by
                'panda.core.Stack::pop' postcondition 'length = @pre(length) - 1'
        7. Test.panda:6:6: precondition 'length > 0' violated

It remains to be seen if the dataflow analysis information can be turned into
sufficiently-sensible English, and if this presentation is actually useful. It 
will be very difficult to strike the right balance of information; for instance,
we might be certain that these 100 lines of the function didn't end up altering
a particular value from its first assignment, but actually explaining that could
involve convoluted proofs of how a particular value isn't visible to the outside
world and how none of the methods we call on it could have affected it, for
*every single such call*. So we probably want to give enough information to 
establish our case, but be smart enough not to have page after page of "and this
call didn't do anything either, because...". Maybe a -pedantic option for
when you really need *all* of the details?

Potential Breaking Changes
==========================

It is not my intent to break the language in any major way... but I'd rather do
the right thing now than be stuck with the wrong approach forever due to
backwards compatibility concerns. So I'm going to make a note of a few things
which I'm not necessarily completely happy with as things stand and which may
change in the future; these will in general require only minor updates to 
existing programs.

Threading
---------

As discussed above, threading is a major candidate for breaking changes.

`switch`
--------

Panda's `switch` statement is fairly... traditional. I haven't spent any time
optimizing it (internally, it is still being lowered to a sequence of `if`
statements) because I am very uncertain if it is going to remain this way.

Interfaces
----------

I am considering requiring a `@default` annotation on default methods in
interfaces. There's no technical reason to require it, of course, but I view it
as similar to the `@override` annotation (which there is also no technical
reason to require): it might improve the quality and clarity of the code to
require it. Or it might not. For the time being, I chose not to require it.

Annotations
-----------

Eventually, I could see adding a general-purpose annotation system. This may
involve updating several of the current annotations which don't "follow the
rules", namely `@math(overflow)` (the token in parentheses is not an 
expression), `@post()` (comes after, rather than before, the thing it 
describes), and `@invariant()` (can appear anywhere in a class body).
`@math(overflow)` will almost certainly change, but I'm less certain about
`@post()` and `@invariant()` -- while they follow their own rules, I kind of
like not having to put them before the method / class they describe. 

Finalization
------------

I do not like Object.finalize. *At all*. It pretends to solve the problem of
cleaning up resources associated with objects, but does not actually do so;
allocating many finalizable objects in a loop will generally break things, 
because they are being allocated faster than they are being finalized and 
whatever native code cares about the object is likely to get upset about that.
Furthermore, such failures are far more likely to happen in the real world,
under heavy load, than they are during carefully-controlled QA testing. This is
bad.

So, I am trying to figure out an alternate approach. Java's try-with-resources /
C#'s using statement are potentially reasonable ways to handle things, probably
with objects that require closing generating a warning if they were destroyed
without being closed (I like that better than silently closing them at 
finalization time, because that tends to hide problems more than it does fix
them. We can have a debug compiler setting which embeds stack traces at 
creation time, to debug where the allocation is happening). Furthermore, the
compiler could easily catch most cases of failure-to-close things, because most
of the time such values will not have escaped and there will be no question that
the close didn't happen.

The only major issue with this is that whether or not an object needs
finalization could be an implementation detail: Matcher, for example, currently
requires closing (because it is backed by a native ICU regex) but will not in
the future (because I'm eventually going to have a Panda-native regex engine).
Some streams don't really require closing, either, such as MemoryOutputStream. 
So, finalization it is for right now, but I'm not happy about it and make no 
guarantees it will remain.

Error Handling
--------------

Panda's exception handling support in general is extremely minimal. I ended up
using Java's basic exception handling approach not because I'm convinced it's
unbelievably awesome, but rather because I haven't seen a compellingly better
way to do it. I haven't spent a tremendous amount of time on exception handling
because I'm not convinced it's the right way to go long-term. In particular,
Panda's `try` / `catch` mostly works (hopefully you didn't notice that it's only 
"mostly"!), but it doesn't support `finally` at all. 

Here are my major concerns:

1. Exception handling is unbelievably slow, particularly when you consider the
   time it takes to assemble stack traces. It's not a big deal when you only
   have exceptions for truly exceptional situations, but when it crops up during
   ordinary day-to-day usage of a program it is a significant performance drain.
2. finally is, in general, a terrible way to handle object cleanup. With a 
   better cleanup system as discussed above, the use cases for finally drop
   dramatically... I'm tempted to just leave it out and see how much people
   complain.