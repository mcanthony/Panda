Implicit Casting
================

In contrast to *explicit* casts performed with the 
[cast operator (`->`)](operators.html#cast), *implicit casts* are performed
automatically by the compiler.

A type may be implicitly cast to another type in the following cases:

* Using an instance of a class where an instance of one of its superclasses is
  expected
* Using a narrower [integer](basicTypes.html#numbers) where a wider integer of 
  the same signedness is expected
* Using `Real32` where `Real64` is expected
* Using an integer where an at-least-as-wide 
  [real number](basicTypes.html#numbers) is expected
* Using an unsigned integer where a wider signed integer is expected
* Using a [null](null.html) where any nullable type is expected
* Using a non-nullable type where the equivalent nullable type is expected
* Using a nullable type where the equivalent non-nullable type is expected, 
  *and* the compiler can prove that the value cannot actually be `null` (see
  [non-nullability](nonNullability.html) for details)
* Using a function type where the equivalent method type is expected
* Using a function or method type which doesn't have the expected return type,
  but the return type can be implicitly cast to the expected return type (for 
  instance the type `(Int)=>(Int)` can be used where the type `(Int)=>(Object)` 
  is expected, because `Int` can be implicitly cast to `Object`).
* Using an immutable method type where a mutable method type is expected
  
For instance, given the method:

@SOURCE(
    method add(o:Object) {
        -*REPLACE:...*---dummy comment
    }
)

is is legal to call `add("Hello!")`, despite the fact that `"Hello!"` is a 
`String` and `add()` expects an `Object`: since `String` is a subclass of
`Object`, an implicit cast from `String` to `Object` is performed.

[Convert parameters](methods.html#convertParameters) are a related concept which
perform automatic conversions during method calls.