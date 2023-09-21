#ifndef NTH_DEBUG_DEBUG_H
#define NTH_DEBUG_DEBUG_H

// Overview:
//
// The "//nth/debug" library consists of a collection of debugging utilities
// useful during development, testing, and release. This library is an
// aggregation of the libraries included below, each of which may be used
// independently. Note however that there are dependencies between the
// libraries, so choosing to depend on just a subset of them may or may not
// reduce executable size and/or compilation-time.

// Logging ("//nth/debug/log"):
//
// This library provides a flexible `NTH_LOG` statement-macro for debug logging.
#include "nth/debug/log/log.h"

// Contracts ("//nth/debug/contracts"):
//
// This library allows user code to define preconditions and postconditions on
// their code and control how/when those conditions are validated.
#include "nth/debug/contracts/contracts.h"

// Properties ("//nth/debug/property"):
//
// This library allows users to define their own properties to be used with both
// test assertions and contracts.
#include "nth/debug/property/property.h"

// Expression Tracing ("//nth/debug/trace"):
//
// Expression tracing is a mechanism that, with minimal or no explicit
// annotation in the code, tracks expression evaluation. This library is
// primarily tool used to provide good error messages as part of the contracts
// library.
#include "nth/debug/trace/trace.h"

// Source Location ("//nth/debug:source_location"):
//
// This library provides a mechanism for accessing source location information
// from within the program. It is a stand-in for the C++20 <source_location>
// standard library.
#include "nth/debug/source_location.h"

// Unimplemented ("//nth/debug:unimplemented"):
//
// This library provides a statement-macro, `NTH_UNIMPLEMENTED` which
// indicates that this particular code path has not yet been implemented.
#include "nth/debug/unimplemented.h"

// Unreachable ("//nth/debug:unimplemented"):
//
// This library provides a statement-macro, `NTH_UNREACHABLE` which portably
// indicates that this statement is not reachable.
#include "nth/debug/unreachable.h"

// Verbosity:
//
// Most of these libraries may be used in a way that is optionally configurable
// by a "verbosity" parameter. The verbosity parameter controls what variant of
// the requested behavior should happen under which circumstances. Each library
// documents precisely how verbosity affects its behavior.
//
// As an example,
//
// ```
// NTH_LOG((v.always), "Log this line no matter what!");
// NTH_LOG((v.debug), "Only log me in debug builds.");
// ```

#endif  // NTH_DEBUG_DEBUG_H
