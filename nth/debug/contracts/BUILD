package(default_visibility = ["//visibility:public"])

cc_library(
    name = "contracts",
    hdrs = ["contracts.h"],
    deps = [
        "//nth/base:section",
        "//nth/debug/contracts/internal:enabler",
        "//nth/debug/contracts/internal:contracts",
        "//nth/debug/trace",
        "//nth/strings:glob",
    ],
)

# TODO: Split this test into tracing and testing for the contract macros.
cc_test(
    name = "contracts_test",
    srcs = ["contracts_test.cc"],
    # We cannot directly test the aborting behavior.
    copts = ["-DNTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST"],
    deps = [
        ":contracts",
        "//nth/debug/internal:raw_check",
        "//nth/debug/log",
        "//nth/debug/log:sink",
        "//nth/debug/log:stderr_log_sink",
    ],
)
