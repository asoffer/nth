# `//nth/format:interpolate`

## Overview

This target provides added functionality on top of [`//nth/format`](/format/format) by making it
possible to specify formatters via [interpolation
strings](https://en.wikipedia.org/wiki/String_interpolation).

Interpolation strings, as used in the `nth` library, are strings with some number of matching braces
`"{...}"`. Each top-level (non-nested) pair of matching braces constitutes a _placeholder_ into which a
value may be formatted. The end result of interpolating will be the interpolation string itself with
each placeholder replaced by some formatted object.

## Basics

The core of the library is the free function-template `nth::interpolate`. It accepts an
`nth::interpolation_string` template parameter, an `nth::io::writer` reference, and a number of
arguments equal to the number of pairs of matching braces.

```
nth::interpolate<"'{}' is the Roman numeral symbol for {}.">(
  nth::io::stdout_writer, "IX", 9);
// "'IX' is the Roman numeral symbol for 9."
```

In the example above, the placeholders are simply `"{}"`, but some types support configuration by
putting text inside the braces. For example, integers can have an `'x'` inside to indicate they
should be formatted in base-16. Strings can have a `'q'` to indicate that they should be escaped as
a quotation.

```
nth::interpolate<"{} can be written is base-16 as {x}.">(
  nth::io::stdout_writer, 17, 17);
// 17 can be written is base-16 as 11."
```

## Builtin format options

The following are the interpolation arguments available for builtin types:

### __`bool`__

* `{}` and `{b}: Lower-case. Writes either `"true"` or `"false"`.
* `{
  B}`: Title-case. Writes either `"True" or `"False"`.
* `{
  B !}`: Upper-case. Writes either `"TRUE" or `"FALSE"`.
* `{
  d}`: Decimal. Writes either `"1" or `"0"`.

### __Integral types__

Integral types are anything satisfying the `std::integral` concept (other than `bool`).

* `{
  d}`: Decimal. Writes the number in base-10.
* `{
  x}`: Hexadecimal. Writes the number in base-16.

### __String-like__

String-like types are anything convertible to `std::string_view`.

* `{}`: Writes the string exactly as-is.
* `{
  q}`: Escaped. Writes the string escaping characters such as `"`, and `\`.

## Custom interpolation formatting.

By default, the empty interpolation placeholder `"{}"` will format an object with that objects
[default formatter](/format/format#custom-default-formatters). However, this behavior is
customizable via a [FTADLE](/ftadle) hook. Specifically, an author of a type `T` may implement a
function template accepting an `nth::interpolation_string` template argument and an
`nth::type_tag<T>` argument. The function, if provided, will be called with the contents that are
between the braces to construct a formatter to be used during interpolation. As an example,

```
struct Person {
  std::string given_name;
  std::string family_name;

  template <nth::interpolation_string S>
  friend auto NthInterpolateFormatter(nth::type_tag<Person>) {
    if constexpr (S.empty()) {
      return full_name_formatter{};
    } else if constexpr (S == "g") {
      return given_name_formatter{};
    } else if constexpr (S == "f") {
      return family_name_formatter{};
    } else {
      // Checked previously so guaranteed to be false (but dependent on the
      // template parameter.
      constexpr bool ValidInterpolationString = S.empty();
      static_assert(ValidInterpolationString,
        "The interpolation string is invalid. It must be "{}", "{
        g}", or "{
        f}".);
    }
  }
};
```

Now, depending on the interpolation string, a person will be formatted with a `full_name_formatter`, a `given_name_formatter` or a `family_name_formatter`. One might implement those as shown below:

```
struct full_name_formatter {
  void format(nth::io::writer auto& w, Person const & p) {
    nth::interpolate<"{} {}">(w, p.given_name, p.family_name);
  }
};

struct given_name_formatter {
  void format(nth::io::writer auto& w, Person const& p) {
    nth::io::write_text(w, p.given_name);
  }
};

struct family_name_formatter {
  void format(nth::io::writer auto& w, Person const& p) {
    nth::io::write_text(w, p.family_name);
  }
};
```

## Escaping

TODO: Escaping has not yet been implemented.

At times one may wish for the interpolation string to have a literal `'{'` character embedded in it
which is not treated as the start of a placeholder. To achieve this, one must escape the character
by prefixing it with a ``'`'`` (and similarly suffixing `'}'` with ``'`'``). If one wishes to have a
literal backtick character, this also needs to be escaped as `"``"` in the interpolation string.

When parsing an interpolation string, the "max munch" rule applies except for escaped closing braces
(because the escape character follows the character to be escaped). In this case escape tokenization
can be thought of as "max much from the right." Alternatively, one could count the number of
backtics following the `'}'` character. The `'}'` is escaped if and only if there are an odd number
of backticks.

