import scalapb.compiler.Version.scalapbVersion

scalaVersion := "2.12.10"

name := "ScalaPractice"
version := "1.0"

// For finding google/protobuf/descriptor.proto
libraryDependencies += "com.thesamet.scalapb" %% "scalapb-runtime" % scalapbVersion % "protobuf"

// https://mvnrepository.com/artifact/com.thesamet.scalapb/scalapb-json4s
libraryDependencies += "com.thesamet.scalapb" %% "scalapb-json4s" % "0.9.3"
libraryDependencies += "com.github.andyglow" %% "websocket-scala-client" % "0.3.0"

mainClass in (Compile, run) := Some("coding.Problems")

// https://github.com/thesamet/sbt-protoc
// PB.protocVersion := "-v3.10.0"
PB.targets in Compile := Seq(
  scalapb.gen() -> (sourceManaged in Compile).value
)

// To disable GRPC
// PB.targets in Compile := Seq(
//   scalapb.gen(grpc=false) -> (sourceManaged in Compile).value
// )

// To generate Java conversion
// PB.targets in Compile := Seq(
//   PB.gens.java -> (sourceManaged in Compile).value,
//   scalapb.gen(javaConversions=true) -> (sourceManaged in Compile).value
// )

// Additional option to generator
// scalapb.gen(
//   flatPackage: Boolean = false,
//   javaConversions: Boolean = false,
//   grpc: Boolean = true,
//   singleLineToProtoString: Boolean = false,
//   asciiFormatToString: Boolean = false,
//   lenses: Boolean = true,
//   retainSourceCodeInfo: Boolean = false
// )
