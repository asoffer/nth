package(default_visibility = ["//visibility:public"])
load(":cc_toolchain_config.bzl", "clang_toolchain")

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
    "-Wno-unused-parameter",
    "-Wno-deprecated",
]

SYSTEM_HEADERS = [
  "absl",
  "clang",
  "clang-tools-extra",
  "llvm",
]

clang_toolchain(
    name = "linux_x86_64_toolchain",
    os = "linux",
    cpu = "x86_64",
    c_compiler_path = "/usr/bin/clang-18",
    cc_compiler_path = "/usr/bin/clang++-18",
    ar_path = "/usr/bin/ar",
    builtin_include_directories = [
        "/usr/lib/llvm-18/lib/clang/18",
        "/usr/include",
    ],
    system_headers = SYSTEM_HEADERS,
    warnings = WARNINGS,
)

clang_toolchain(
    name = "macos_arm64_toolchain",
    os = "macos",
    cpu = "arm64",
    c_compiler_path = "/opt/local/bin/clang-mp-18",
    cc_compiler_path = "/opt/local/bin/clang++-mp-18",
    ar_path = "/opt/local/bin/llvm-ar-mp-18",
    builtin_include_directories = [
        "/opt/local/libexec/llvm-18/include/c++/v1",
        "/opt/local/libexec/llvm-18/lib/clang/18",
        "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk",
    ],
    system_headers = SYSTEM_HEADERS,
    warnings = WARNINGS,
)

clang_toolchain(
    name = "macos_x86_64_toolchain",
    os = "macos",
    cpu = "x86_64",
    c_compiler_path = "/opt/local/bin/clang-mp-18",
    cc_compiler_path = "/opt/local/bin/clang++-mp-18",
    ar_path = "/opt/local/bin/llvm-ar-mp-18",
    builtin_include_directories = [
        "/opt/local/libexec/llvm-18/include/c++/v1",
        "/opt/local/libexec/llvm-18/lib/clang/18",
        "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk",
    ],
    system_headers = SYSTEM_HEADERS,
    warnings = WARNINGS,
)

config_setting(
    name = "dbg",
    values = {"compilation_mode": "dbg"},
)

config_setting(
    name = "opt",
    values = {"compilation_mode": "opt"},
)
