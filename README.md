# NTH
[![CI badge](https://github.com/asoffer/nth/workflows/CI/badge.svg)](https://github.com/asoffer/nth/actions?query=workflow%3ACI)

A collection of libraries I find generally useful. More explanation to follow.

## Building from source

`nth` is a collection of C++20 libraries, and relies heavily on Bazel for
building and testing. You will need a C++20-compliant compiler and a relatively
recent copy of [Bazel](https://bazel.build/). Specifically,

* Bazel version 5.4.0 or greater
* Clang 14.0.0

The `nth` libraries are intended to be used from your C++ code and always built
from source. For this reason, we does not provide any installation or setup
process. If you would like to download and play around with the examples, they
can be built via these commands:

```
$ git clone https://github.com/asoffer/nth.git
$ cd nth
$ bazel test ...
```

If you would like to include `nth` in your own Bazel based project, you can
use an [`http_archive`](https://bazel.build/rules/lib/repo/http) rule in your
`WORKSPACE` file. See Bazel's tutorials for how to do this. A good starting
point will be to copy this into your `WORKSPACE` file:

```
http_archive(
  name = "asoffer_nth",
  urls = ["https://github.com/asoffer/nth/archive/<<hash>>.zip"],
  strip_prefix = "nth-<<hash>>",
  sha256 = "<<sha>>",
)
```

where `<<hash>>` and `<sha>>` are replaced with the appropriate Git hash and
checksum as desired.

## What can I find in this library?

Anything I find generally useful across more than one project.

## Long term support

As it is currently in the early exploratory stages of development, we are not
ready to make any guarantees, even for short-term stability.

In the future we intend to provide significant long-term support, but in a
somewhat unconventional way: `nth` will never make guarantees of either API
stability or ABI stability. While we aim to make API changes unobtrusive, we
will add, change, or remove APIs. This enables us to always be able to provide
the best possible software for our users.

Though any change can be a breaking change, we aim to advertize the potential
breakge (e.g., removing a public function). Moreover, we aim to make such
changes in such a way that upgrading the version of `nth` can be done in a
change of its own, separate from any refactoring that may be required to make
the upgrade. Concretely, this means that if we want to, for example, rename a
function, we will add a function with the new name and mark the old one as
deprecated in one change, and remove the deprecated function in a separate
change.

## Why is it called `nth`?

Mostly because I use a Dvorak keyboard layout, and those three characters in
that order are particularly easy to type. It also has the accurate connotation
that this is yet another kitchen sink utility library, but I didn't realize that
until first mashing on the keyboard.
