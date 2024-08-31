package(default_visibility = ["//visibility:public"])

cc_library(
    name = "contract",
    hdrs = ["contract.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:indestructible",
        "//nth/debug:source_location",
        "//nth/io/format",
        "//nth/format/interpolate:string_view",
        "//nth/io/writer",
        "//nth/io/writer:string",
        "//nth/registration:registrar",
    ],
)

cc_library(
    name = "contracts",
    hdrs = ["contracts.h"],
    deps = [
        ":violation",
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
        "//nth/debug/log:file_log_sink",
    ],
)

cc_library(
    name = "violation",
    hdrs = ["violation.h"],
    srcs = ["violation.cc"],
    deps = [
        ":contract",
        "//nth/base:attributes",
        "//nth/base:indestructible",
        "//nth/debug:source_location",
        "//nth/io/format",
        "//nth/format/interpolate:string_view",
        "//nth/io/writer",
        "//nth/io/writer:string",
        "//nth/registration:registrar",
    ],
)
