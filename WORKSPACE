workspace(name = "asoffer_nth")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    sha256 = "a22290c26d29d3ecca286466f7f295ac6cbe32c0a9da3a91176a90e0725e3649",
    strip_prefix = "bazel-skylib-5bfcb1a684550626ce138fe0fe8f5f702b3764c3",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/5bfcb1a684550626ce138fe0fe8f5f702b3764c3.zip"],
)

http_archive(
  name = "com_google_absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/bb63a76710554cebbeb20306739a7b832be38c4a.zip"],
  strip_prefix = "abseil-cpp-bb63a76710554cebbeb20306739a7b832be38c4a",
  sha256 = "91209b5eecd9c3d862b230fefbc2728c7f2764ff6d5866ec398d48db1aaa1e90",
)

http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/d166e09483845b9b6a658dccc3d3dbb293676b62.zip"],
  strip_prefix = "googletest-d166e09483845b9b6a658dccc3d3dbb293676b62",
  sha256 = "d27641a853c49d3e8d7b9bbced1ceb861336134cd148bf6db720a40ccde66516",
)
