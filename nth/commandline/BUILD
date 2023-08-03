package(default_visibility = ["//visibility:public"])

cc_library(
    name = "commandline",
    hdrs = ["commandline.h"],
    srcs = [
        "commandline.cc",
        "internal/invoke.h",
        "internal/invoke.cc",
        "internal/parse.h",
        "internal/parse.cc",
    ],
    deps = [
        "//nth/debug",
        "//nth/debug/log",
        "//nth/io:file_printer",
        "//nth/io:printer",
        "//nth/process:exit_code",
        "//nth/strings:text",
        "//nth/utility:required",
    ],
)

cc_test(
    name = "invoke_test",
    srcs = ["invoke_test.cc"],
    deps = [
        ":commandline",
        ":rock_paper_scissors",
        "//nth/test:main",
    ],
)

cc_test(
    name = "parse_test",
    srcs = ["parse_test.cc"],
    deps = [
        ":commandline",
        ":rock_paper_scissors",
        "//nth/test:main",
    ],
)

cc_library(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":commandline",
        "//nth/debug/log",
        "//nth/debug/log:stderr_log_sink",
    ],
)

cc_library(
    name = "rock_paper_scissors",
    hdrs = ["internal/rock_paper_scissors.h"],
    testonly = True,
    visibility = ["//visibility:private"],
    deps = [],
)
