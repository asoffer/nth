package(default_visibility = ["//visibility:public"])

cc_library(
    name = "deserialize",
    hdrs = ["deserialize.h"],
    deps = [
        ":reader",
        ":sequence",
        "//nth/container:free_functions",
        "//nth/meta:concepts",
        "//nth/utility:bytes",
    ],
)

cc_library(
    name = "reader",
    hdrs = ["reader.h"],
    deps = [],
)

cc_library(
    name = "reader_test",
    testonly = True,
    hdrs = ["reader_test.h"],
    deps = [
        ":reader",
        "//nth/test",
    ],
)

cc_test(
    name = "round_trip_test",
    srcs = ["round_trip_test.cc"],
    deps = [
        ":deserialize",
        ":string_reader",
        ":string_writer",
        ":serialize",
        "//nth/container:flyweight_set",
        "//nth/test:main",
    ],
)

cc_library(
    name = "serialize",
    hdrs = ["serialize.h"],
    deps = [
        ":sequence",
        ":writer",
        "//nth/container:free_functions",
        "//nth/utility:bytes",
    ],
)

cc_library(
    name = "sequence",
    hdrs = ["sequence.h"],
    visibility = ["//visibility:private"],
    deps = [],
)

cc_library(
    name = "string_reader",
    hdrs = ["string_reader.h"],
    srcs = ["string_reader.cc"],
    deps = [":reader"],
)

cc_test(
    name = "string_reader_test",
    srcs = ["string_reader_test.cc"],
    deps = [
        ":reader_test",
        ":string_reader",
        "//nth/test:main",
    ],
)

cc_library(
    name = "string_writer",
    hdrs = ["string_writer.h"],
    srcs = ["string_writer.cc"],
    deps = [":writer"],
)

cc_test(
    name = "string_writer_test",
    srcs = ["string_writer_test.cc"],
    deps = [
        ":writer_test",
        ":string_writer",
        "//nth/test:main",
    ],
)

cc_library(
    name = "writer",
    hdrs = ["writer.h"],
    deps = [
        "//nth/utility:bytes",
    ],
)

cc_library(
    name = "writer_test",
    testonly = True,
    hdrs = ["writer_test.h"],
    deps = [
        ":writer",
        "//nth/test",
    ],
)
