package(default_visibility = ["//visibility:public"])

cc_library(
    name = "arguments",
    hdrs = ["arguments.h"],
    visibility = ["//visibility:private"],
    testonly = True,
    deps = [
      "//nth/test/internal:arguments",
      "//nth/test/internal:invocation",
    ],
)

cc_test(
    name = "arguments_test",
    srcs = ["arguments_test.cc"],
    deps = [
        ":arguments",
    ],
)

cc_library(
    name = "benchmark",
    testonly = True,
    hdrs = ["benchmark.h"],
    deps = [
        "//nth/test/internal:benchmark",
    ],
)

cc_library(
    name = "benchmark_result",
    testonly = True,
    hdrs = ["benchmark_result.h"],
    srcs = ["benchmark_result.cc"],
    deps = [
        "//nth/base/internal:global_registry",
    ],
)

cc_library(
    name = "fakeable_function",
    testonly = True,
    hdrs = ["fakeable_function.h"],
    deps = [
        "//nth/base:attributes",
        "//nth/base:macros",
        "//nth/debug:fakeable_function",
        "//nth/meta:type",
        "//nth/utility:any_invocable",
    ],
)

cc_library(
    name = "fuzz",
    testonly = True,
    hdrs = ["fuzz.h"],
    deps = [
        ":generator",
        "@com_google_absl//absl/random",
    ],
)

cc_library(
    name = "generator",
    testonly = True,
    hdrs = ["generator.h"],
    deps = [
        ":arguments",
    ],
)

cc_library(
    name = "test",
    hdrs = ["test.h"],
    srcs = ["test.cc"],
    testonly = True,
    deps = [
        ":arguments",
        ":fakeable_function",
        ":fuzz",
        "//nth/base:macros",
        "//nth/debug/property",
        "//nth/debug/contracts",
        "//nth/test/internal:test",
    ],
)

cc_library(
    name = "main",
    srcs = ["main.cc"],
    testonly = True,
    deps = [
        ":benchmark",
        ":benchmark_result",
        ":test",
        "//nth/debug/log",
        "//nth/debug/log:stderr_log_sink",
        "//nth/utility:no_destructor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "test_test",
    srcs = ["test_test.cc"],
    deps = [
        ":test"
    ],
)
