# `//nth/base:indestructible`

An `nth::indestructible<T>` is a wrapper around an object of type `T` which does not invoke the
destructor of `T` when it is destroyed.  The primary motivation for such a type is for non-trivial
static globals; such types frequently do not need to have their destructors run (as resources are
returned to the operating system on exit). Moreover, running these destructors is typically fraught,
if references to these object are held by multiple threads.
