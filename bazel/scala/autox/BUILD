package(default_visibility = ["//visibility:public"])

load("@io_bazel_rules_scala//scala:scala.bzl", "scala_binary", "scala_library", "scala_test")

scala_binary(
    name = "hello",
    main_class = "autox.Hello",
    srcs = ["Hello.scala"],
    deps = [
        "//proto:hello_proto_java",
        "@maven//:com_google_guava_guava",
        "@maven//:com_hivemq_hivemq_mqtt_client",
        "@maven//:io_github_sac_SocketclusterClientJava",
    ],
)
