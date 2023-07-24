#include "nth/strings/glob.h"

namespace nth {

static_assert(GlobMatches("", ""));
static_assert(not GlobMatches("", "abc"));
static_assert(not GlobMatches("", "abc/def"));

static_assert(GlobMatches("*", ""));
static_assert(GlobMatches("*", "abc"));

static_assert(not GlobMatches("*", "/"));
static_assert(not GlobMatches("*", "/abc"));
static_assert(not GlobMatches("*", "abc/"));
static_assert(not GlobMatches("*", "abc/def"));

static_assert(not GlobMatches("*/*", "abc"));
static_assert(GlobMatches("*/*", "/"));
static_assert(GlobMatches("*/*", "/abc"));
static_assert(GlobMatches("*/*", "abc/"));
static_assert(GlobMatches("*/*", "abc/def"));

static_assert(GlobMatches("**", "abc"));
static_assert(GlobMatches("**", "/"));
static_assert(GlobMatches("**", "/abc"));
static_assert(GlobMatches("**", "abc/"));
static_assert(GlobMatches("**", "abc/def"));

static_assert(GlobMatches("**/x", "abc/x"));
static_assert(GlobMatches("**/x", "/x"));
static_assert(GlobMatches("**/x", "/abc/x"));
static_assert(GlobMatches("**/x", "abc/x"));
static_assert(GlobMatches("**/x", "abc/def/x"));
static_assert(not GlobMatches("**/x", "abc/y"));
static_assert(not GlobMatches("**/x", "/y"));
static_assert(not GlobMatches("**/x", "/abc/y"));
static_assert(not GlobMatches("**/x", "abc/y"));
static_assert(not GlobMatches("**/x", "abc/def/y"));

static_assert(not GlobMatches("?", ""));
static_assert(GlobMatches("?/?", "a/b"));
static_assert(not GlobMatches("?/?", "a/bc"));
static_assert(GlobMatches("a?c", "abc"));
static_assert(GlobMatches("a?c", "adc"));
static_assert(not GlobMatches("a?c", "ac"));
static_assert(GlobMatches("?", "?"));

}  // namespace nth

int main() { return 0; }
