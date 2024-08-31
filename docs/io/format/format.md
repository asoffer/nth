# `//nth/format`

## Overview

The formatting library is a layer on top of [`//nth/io/writer`](/io/writer/writer) responsible for
marhsalling data of arbitrary types into a writer. The primary workhorse is the `nth::io::format`
function template, which accepts a `nth::io::writer`, a formatter, and an object to be formatted.

There is also a two-parameter overload for `nth::io::format`, which uses `nth::default_formatter<T>`
as its formatter.

## `nth::io::format`

The `nth::io::format` function template accepts a reference to a writer `w`, a reference to a
formatter `fmt`, and a const-reference to a value `v` to be formatted. If `fmt.format(w, value)` is
a valid expression, this is what `nth::io::format(w, fmt, v)` evaluates to. Otherwise, it will
evaulate to `NthFormat(w, fmt, value)`, where `NthFormat` is the [FTADLE](/ftadle) hook.

As an example, consider a `Person` struct and suppose we would like to implement a function that
returns a greeting string. To do this we would implement `NthFormat` for the `Person` struct.

```
struct Person {
  std::string name;

  friend void NthFormat(nth::io::writer auto& w, auto& fmt, Person const & p) {
    nth::io::format(w, fmt, p.name);
  }
};

std::string greeting_string(Person const & person) {
  std::string s;
  nth::io::string_writer w(s);
  nth::io::write_text(w, "Hello, ");
  nth::io::format(w, person);
  nth::io::write_text(w, "!");
  return s;
}
```

Note in this example `nth::io::format(w, person)` uses the default formatter for `Person` (because
the two-parameter `nth::io::format` is invoked). The default formatter for person is
`nth::io::trivial_formatter` (more on how to customize this below), which does not have a `format`
member function, so `NthFormat(w, fmt, p)` is invoked. This in turn invokes `nth::io::format(w, fmt,
p.name)`. This process continues recursively, now with the same writer, an
`nth::io::trivial_formatter` for the formatter, and `p.name` of type `std::string`. Because the
library provides a FTADLE hook for `std::string`, this resolves by writing the string to the writer.

## Custom default formatters

By default, the default formatter for a type is `nth::io::trivial_formatter`, but users may
configure the default formatter (the one used when no formatter argument is provided to
`nth::io::format`) with a FTADLE hook. In particular, users can implement a function named
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
    nth::io::format(w, fmt, p.name);
  }
};
```

Now, when a person is formatted, the name will be capitalized by default, though one could pass a
different `PersonFormatter` to achieve a different result.
