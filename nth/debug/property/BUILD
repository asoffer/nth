package(default_visibility = ["//visibility:public"])

cc_library(
    name = "property",
    hdrs = ["property.h"],
    deps = [
        "//nth/debug/property/internal:concepts",
        "//nth/debug/property/internal:implementation",
        "//nth/debug/contracts",
        "//nth/meta/concepts:invocable",
    ],
)

cc_test(
    name = "property_test",
    srcs = ["property_test.cc"],
    deps = [
        ":property",
        "//nth/debug:expectation_result",
        "//nth/debug/contracts",
    ],
)
