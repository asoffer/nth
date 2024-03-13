package(default_visibility = ["//visibility:public"])

cc_library(
    name = "c_array",
    hdrs = ["c_array.h"],
    deps = [],
)

cc_test(
    name = "c_array_test",
    srcs = ["c_array_test.cc"],
    deps = [":c_array"],
)

cc_library(
    name = "comparable",
    hdrs = ["comparable.h"],
    deps = [":core"],
)

cc_test(
    name = "comparable_test",
    srcs = ["comparable_test.cc"],
    deps = [":comparable"],
)

cc_library(
    name = "constructible",
    hdrs = ["constructible.h"],
    deps = [],
)

cc_test(
    name = "constructible_test",
    srcs = ["constructible_test.cc"],
    deps = [":constructible"],
)

cc_library(
    name = "core",
    hdrs = ["core.h"],
    deps = [],
)

cc_test(
    name = "core_test",
    srcs = ["core_test.cc"],
    deps = [":core"],
)
