package(default_visibility = ["//visibility:private"])

cc_library(
    name = "global_registry",
    hdrs = ["global_registry.h"],
    deps = [
        "//nth/utility:no_destructor",
        "@abseil-cpp//absl/synchronization",
    ],
    visibility = [
        "//nth/debug:__pkg__",
        "//nth/test:__pkg__",
    ],
)

cc_test(
    name = "global_registry_test",
    srcs = ["global_registry_test.cc"],
    deps = [
        ":global_registry",
        "//nth/debug/internal:raw_check",
    ],
)
