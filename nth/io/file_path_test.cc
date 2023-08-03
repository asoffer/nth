#include "nth/io/file_path.h"

#include "nth/test/test.h"

namespace nth {

NTH_TEST("file_path/canonicalization") {
  NTH_EXPECT(file_path::try_construct("") == file_path::try_construct(""));
  NTH_EXPECT(file_path::try_construct("a") == file_path::try_construct("a"));
  NTH_EXPECT(file_path::try_construct("/") == file_path::try_construct("/"));
  NTH_EXPECT(file_path::try_construct("a") != file_path::try_construct("b"));
  NTH_EXPECT(file_path::try_construct("a/a") !=
             file_path::try_construct("a/b"));
  NTH_EXPECT(file_path::try_construct("a/b") !=
             file_path::try_construct("b/b"));

  NTH_EXPECT(file_path::try_construct("./") == file_path::try_construct(""));
  NTH_EXPECT(file_path::try_construct("./a") == file_path::try_construct("a"));
  NTH_EXPECT(file_path::try_construct("././") == file_path::try_construct(""));
  NTH_EXPECT(file_path::try_construct("a/.") == file_path::try_construct("a"));
  NTH_EXPECT(file_path::try_construct("./a/.") ==
             file_path::try_construct("a"));
  NTH_EXPECT(file_path::try_construct("a/./.") ==
             file_path::try_construct("a"));

  NTH_EXPECT(file_path::try_construct("a/..") == file_path::try_construct(""));
  NTH_EXPECT(file_path::try_construct("a/b/..") ==
             file_path::try_construct("a/"));
  NTH_EXPECT(file_path::try_construct("a/b/../c/d") ==
             file_path::try_construct("a/c/d"));
  NTH_EXPECT(file_path::try_construct("a/b/.././d") ==
             file_path::try_construct("a/d"));
  NTH_EXPECT(file_path::try_construct("a/./../c/d") ==
             file_path::try_construct("c/d"));

  NTH_EXPECT(file_path::try_construct("") != std::nullopt);
  NTH_EXPECT(file_path::try_construct("a") != std::nullopt);
  NTH_EXPECT(file_path::try_construct("./") != std::nullopt);
  NTH_EXPECT(file_path::try_construct("a/.") != std::nullopt);
  NTH_EXPECT(file_path::try_construct("a/..") != std::nullopt);
  NTH_EXPECT(file_path::try_construct("/") == std::nullopt);
  NTH_EXPECT(file_path::try_construct("//") == std::nullopt);
  NTH_EXPECT(file_path::try_construct("/..") == std::nullopt);
  NTH_EXPECT(file_path::try_construct("./../") == std::nullopt);
  NTH_EXPECT(file_path::try_construct("a/../..") == std::nullopt);
  NTH_EXPECT(file_path::try_construct("a//..") == std::nullopt);
}

NTH_TEST("file_path/c-string") {
  NTH_EXPECT(file_path::try_construct("a/b/c")->path() == "a/b/c");
  NTH_EXPECT(file_path::try_construct("a/./c")->path() == "a/c");
  NTH_EXPECT(file_path::try_construct("a/../c")->path() == "c");
  NTH_EXPECT(file_path::try_construct("./b/c")->path() == "b/c");
  NTH_EXPECT(file_path::try_construct("a/b/.")->path() == "a/b");
}
}  // namespace nth
