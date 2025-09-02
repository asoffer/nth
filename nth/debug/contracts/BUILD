package(default_visibility = ["//visibility:public"])

cc_library(
    name = "contract",
    hdrs = ["contract.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:indestructible",
        "//nth/debug:source_location",
        "//nth/format",
        "//nth/format:interpolate",
        "//nth/io/writer",
        "//nth/io/writer:string",
        "//nth/registration:registrar",
    ],
)

cc_library(
    name = "contracts",
    hdrs = ["contracts.h"],
    srcs = ["contracts.cc"],
    deps = [
        "//nth/base:section",
        "//nth/debug/contracts/internal:contracts",
        "//nth/strings:glob",
        ":registry.ld",
    ],
    linkopts = [
        "-Wl,-T,$(location registry.ld)",
    ],
    data = ["registry.ld"],
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
        "//nth/debug/contracts/internal:any_formattable_ref",
        "//nth/registration:registrar",
    ],
)
