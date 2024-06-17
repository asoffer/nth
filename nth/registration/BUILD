package(default_visibility = ["//visibility:public"])

cc_library(
    name = "registrar",
    hdrs = ["registrar.h"],
    deps = [
        "//nth/registration/internal:registry_node",
        "@abseil-cpp//absl/synchronization",
    ],
)

cc_test(
    name = "registrar_test",
    srcs = ["registrar_test.cc"],
    deps = [
        ":registrar",
        "//nth/test/raw:test",
    ],
)

cc_library(
    name = "static",
    hdrs = ["static.h"],
    deps = [],
)
