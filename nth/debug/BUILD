package(default_visibility = ["//visibility:public"])

cc_library(
    name = "config",
    hdrs = ["config.h"],
    srcs = ["config.cc"],
    deps = [
        ":verbosity",
        "//nth/configuration",
    ],
)

cc_library(
    name = "source_location",
    hdrs = ["source_location.h"],
    deps = [
        "//nth/utility:unconstructible",
    ],
)

cc_test(
    name = "source_location_test",
    srcs = ["source_location_test.cc"],
    deps = [
        ":source_location",
    ],
)

cc_library(
    name = "trace",
    hdrs = ["trace.h"],
    srcs = [
        "internal/trace.h",
        "internal/trace_declare_api.h",
        "internal/trace_operators.h",
    ],
    deps = [
        ":config",
        ":source_location",
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/meta:type",
        "//nth/io:string_printer",
        "//nth/io:universal_print",
        "//nth/strings:format",
    ],
)

cc_test(
    name = "trace_test",
    srcs = ["trace_test.cc"],
    deps = [
        ":trace",
    ],
)

cc_library(
    name = "verbosity",
    hdrs = ["verbosity.h"],
    deps = [
        ":source_location",
        "//nth/base:attributes",
    ],
)


