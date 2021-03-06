Constants
=========

A *constant* is a value which cannot change at runtime. Constants are introduced
with the syntax

@SOURCE(
    constant -*REPLACE:<name>*-x-*REPLACE:[*- :-*REPLACE:<type>*-Int-*REPLACE:] *- := -*REPLACE:<value>*-5
)

By convention, the names of constants are written entirely in uppercase, using
underscores to separate words. For instance,

@SOURCE(
    constant DAYS_IN_WEEK := 7
)

To enforce the "cannot change" rule, all constants must have 
[immutable](immutable.html) types. Constants may appear either in class bodies, 
at the same level as methods and fields, or within methods. Constants in class
bodies are scoped to the class, for instance:

@SOURCE(
    class Physics {
        constant C := 299792458
        constant H := 6.62606957e-34
    }

    Console.writeLine("The speed of light is \{Physics.C} m/s")
)

Constants appearing in methods are treated much like local 
[variables](variables.html), except that they are read-only and must be 
immutable. 

Constants in general are similar to [defines](defines.html); the difference is
that constants may be class-scoped and must be immutable, whereas defines may at
most be instance scoped and may be either mutable or immutable.