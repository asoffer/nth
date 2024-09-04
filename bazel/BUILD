package(default_visibility = ["//visibility:public"])
load(":cc_toolchain_config.bzl", "cc_toolchain_config", "clang_toolchain")

filegroup(name = "empty")

WARNINGS = [
    "-Wall",
    "-Werror",
    "-Wextra",
    "-Wpedantic",
    "-Wuninitialized",
    "-Wno-c++2b-extensions",
    "-Wno-deprecated-declarations",
    "-Wno-gcc-compat",
    "-Wno-gnu-statement-expression-from-macro-expansion",
    "-Wno-missing-field-initializers",
]

clang_toolchain(
    os = "linux",
    cpu = "x86_64",
    compiler_path = "/usr/bin/clang++-18",
    warnings = WARNINGS,
)

clang_toolchain(
    os = "macos",
    cpu = "x86_64",
    compiler_path = "/usr/local/opt/llvm/bin/clang",
    warnings = WARNINGS,
)

clang_toolchain(
    os = "macos",
    cpu = "arm64",
    compiler_path = "/opt/local/bin/clang++-mp-18",
    warnings = WARNINGS,
)
