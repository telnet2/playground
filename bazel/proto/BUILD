package(default_visibility = ["//visibility:public"])

load("@io_bazel_rules_scala//scala_proto:scala_proto.bzl", "scalapb_proto_library")

proto_library(
    name = "hello_proto",
    srcs = ["autox/hello.proto"],
)

cc_proto_library(
    name = "hello_proto_cc",
    deps = [":hello_proto"],
)

# java_proto_library(
#     name = "hello_proto_java",
#     deps = [":hello_proto"],
# )

scalapb_proto_library(
    name = "hello_proto_scala",
    deps = [":hello_proto"],
)
