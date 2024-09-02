# `//nth/format`

## Overview

The formatting library is a layer on top of [`//nth/io/writer`](/io/writer/writer) responsible for
marhsalling data of arbitrary types into a writer. The primary workhorse is the `nth::format`
function template, which accepts a `nth::io::writer`, a formatter, and an object to be formatted.

There is also a two-parameter overload for `nth::format`, which uses `nth::default_formatter<T>`
as its formatter.

## `nth::format`

The `nth::format` function template accepts a reference to a writer `w`, a reference to a
formatter `fmt`, and a const-reference to a value `v` to be formatted. If `fmt.format(w, value)` is
a valid expression, this is what `nth::format(w, fmt, v)` evaluates to. Otherwise, it will
evaulate to `NthFormat(w, fmt, value)`, where `NthFormat` is the [FTADLE](/ftadle) hook.

As an example, consider a `Person` struct and suppose we would like to implement a function that
returns a greeting string. To do this we would implement `NthFormat` for the `Person` struct.

```
struct Person {
  std::string name;

  friend void NthFormat(nth::io::writer auto& w, auto& fmt, Person const & p) {
    nth::format(w, fmt, p.name);
  }
};

std::string greeting_string(Person const & person) {
  std::string s;
  nth::io::string_writer w(s);
  nth::io::write_text(w, "Hello, ");
  nth::format(w, person);
  nth::io::write_text(w, "!");
  return s;
}
```

Note in this example `nth::format(w, person)` uses the default formatter for `Person` (because
the two-parameter `nth::format` is invoked). The default formatter for person is
`nth::trivial_formatter` (more on how to customize this below), which does not have a `format`
member function, so `NthFormat(w, fmt, p)` is invoked. This in turn invokes `nth::format(w, fmt,
p.name)`. This process continues recursively, now with the same writer, an
`nth::trivial_formatter` for the formatter, and `p.name` of type `std::string`. Because the
library provides a FTADLE hook for `std::string`, this resolves by writing the string to the writer.

## Custom default formatters

By default, the default formatter for a type is `nth::trivial_formatter`, but users may
configure the default formatter (the one used when no formatter argument is provided to
`nth::format`) with a FTADLE hook. In particular, users can implement a function named
`NthDefaultFormatter` accepting an `nth::type_tag<T>`. Note that this function can also be a hidden
friend inside the body of the type `T`, as this will still be findable via argument-dependent
lookup. As an example, one might want a formatter fora person to distinguish whether or not their
name should be capitalized. One could add the following to make the default capitaziled:

```
struct PersonFormatter {
  bool capitalized;

  void format(nth::io::writer auto& w, Person const & p) const {
    if (p.name.empty()) { return; }
    if (capitalized) {
      nth::io::write_text(w, capitalize(p.name[0])); 
      nth::io::write_text(w, p.name.substr(1));
    } else {
      nth::io::write_text(w, p.name);
    }
  }
};

struct Person {
  std::string name;

  friend PersonFormatter NthDefaultFormatter(nth::type_tag<Person>) {
    return PersonFormatter{.capitalized = true};
  }

  friend void NthFormat(nth::io::writer auto& w, auto& fmt, Person const & p) {
    nth::format(w, fmt, p.name);
  }
};
```

Now, when a person is formatted, the name will be capitalized by default, though one could pass a
different `PersonFormatter` to achieve a different result.

## Structured Formatting

A common formatting pattern is to effectively format all of the content in a struct, sequence, or
container, possibly with some surrounding decaration. The `nth::structural_formatter` type
can be used to simplify this process.

To use the`nth::structural_formatter` type, users must inherit from it and provide several
customization points. The first such hook is a static variable template of type
[`nth::structure`](/types/structure) named `structure_of`, as shown in the example below:

```
struct my_formatter : nth::structural_formatter {
    template <typename T>
    static constexpr nth::structure structure_of = ...

    ...
};
```

The base class will use `structure_of` to understand the structure of each to-be-formatted type and
do the formatting accordingly. Users may also implement, for each enumerator `S` in `nth::structure`, a
member function template in their formatter with signatures

* `begin(nth::constant_value<S>, nth::io::writer auto& w)` and
* `end(nth::constant_value<S>, nth::io::writer auto& w)`,

to be called immediately before and after (respectively) an object of that structural category is
formatted. This allows one to, for instance, wrap sequences in `"[...]"`, or associative containers in
`"{...}"`.

Note that if you implement `begin` for a structural category, you must also implement `end` (even if
it is empty), and vice versa. Failing to do so will result in a compilation error with an error
message explaining the mistake. This is an intentional design choice. Due to the nature of the
design, it is easy to accidentally forget or misspell one of these function templates and silently
leave off such a function. While we would hope that unit tests would catch such problems, enforcing
that users must provide _both_ or _neither_ yields slightly more assurance of correctness.

### Sequences
When sequences are formatted via `nth::structural_formatter`, the following will occur.

```
nth::begin_format<nth::structure::sequence>(w, fmt);
for (auto const& element : seq) {
  nth::begin_format<nth::structure::entry>(w, fmt);
  nth::format(w, fmt, element);
  nth::end_format<nth::structure::entry>(w, fmt);
}
nth::end_format<nth::structure::sequence>(w, fmt);
```

### Associative containers
When associative containers are formatted via `nth::structural_formatter`, the following will occur.

```
nth::begin_format<nth::structure::associative>(w, fmt);
for (auto const& [k, v] : associative_container) {
  nth::begin_format<nth::structure::key>(w, fmt);
  nth::format(w, fmt, k);
  nth::end_format<nth::structure::key>(w, fmt);

  nth::begin_format<nth::structure::value>(w, fmt);
  nth::format(w, fmt, v);
  nth::end_format<nth::structure::value>(w, fmt);
}
nth::end_format<nth::structure::associative>(w, fmt);
```

### Objects

There is no provided builtin mechanism for formatting objects.
