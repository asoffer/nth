package(default_visibility = ["//visibility:public"])

cc_library(
    name = "signal",
    hdrs = ["signal.h"],
    srcs = [
        "signal.cc",
        "signal.xmacro.h",
    ],
)

cc_library(
    name = "exit_code",
    hdrs = ["exit_code.h"],
    srcs = [
        "exit_code.cc",
        "exit_code.xmacro.h",
    ],
    deps = [
        ":signal",
    ],
)
