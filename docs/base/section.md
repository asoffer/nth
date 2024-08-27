## `//nth/base:section`

Provides an API for defining sections in a binary and placing symbols in those sections.

A section may be declared with `NTH_DECLARE_SECTION(my_section_name, some_type)`, indicating that
the section is named `"my_section_name"` and that all elements in that section will have type
`some_type`. Sections holding heterogeneously-typed objects are not supported.

One may query this element-type of a section via `nth::section_type<"my_section_name">`.

Once a section has been declared, one may place objects in the section by prefixing their
declaration with `NTH_PLACE_IN_SECTION(my_section_name)`.

The expression `nth::section<"my_section_name">` provides `begin` and `end` member functions and may
be iterated over via a range-for loop.

