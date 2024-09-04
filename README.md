# NTH
[![CI badge](https://github.com/asoffer/nth/workflows/CI/badge.svg)](https://github.com/asoffer/nth/actions?query=workflow%3ACI)

A collection of libraries I find generally useful. Broadly speaking, `nth` aims at providing
high-quality, highly-customizable libraries to be used across a broad number of applications. Some
of these libraries are additive to the C++ standard library, while others are
replacements/alternatives.

## Building from source

`nth` is a collection of C++23 libraries, and relies heavily on Bazel for building and testing. You
will need a C++23-compliant compiler and a relatively recent copy of [Bazel](https://bazel.build/).
Specifically,

* Bazel version 7.3.1 or greater
* Clang 18.0.0

The `nth` libraries are intended to be used from your C++ code and always built from source. For
this reason, we do not provide any installation or setup process. If you would like to download and
play around with the examples, they can be built via these commands:

```
$ git clone https://github.com/asoffer/nth.git
$ cd nth
$ bazel test ...
```

If you would like to include `nth` in your own Bazel-based project, you can consume it as a [Bazel
module](). I host a [Bazel
repository](https://raw.githubusercontent.com/asoffer/bazel-registry/main) and update it somewhat
regularly. You are of course always welcome to host your own Bazel repository pointing
to the source on this github repository.

## What can I find in this library?

Anything I find generally useful across more than one project.

## Why is it called `nth`?

Mostly because I use a Dvorak keyboard layout, and those three characters in
that order are particularly easy to type. It also has the accurate connotation
that this is yet another kitchen sink utility library, but I didn't realize that
until first mashing on the keyboard.
