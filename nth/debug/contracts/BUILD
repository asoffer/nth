package(default_visibility = ["//visibility:public"])

cc_library(
    name = "contracts",
    hdrs = ["contracts.h"],
    deps = [
        "//nth/debug/trace",
        "//nth/debug/contracts/internal:contracts"
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
    ],
)
