// import scalapb.compiler.Version.scalapbVersion

name := "Line Counter"
version := "1.0"
scalaVersion := "2.11.12"
// showSuccess := false
logLevel := Level.Error

// Compile Protobufs
//PB.targets in Compile := Seq(
//  scalapb.gen() -> (sourceManaged in Compile).value
//)

PB.targets in Compile := Seq(
  PB.gens.java -> (sourceManaged in Compile).value
)

libraryDependencies += "org.apache.spark" % "spark-core_2.11" % "2.4.4"
libraryDependencies += "org.apache.spark" % "spark-sql_2.11" % "2.4.4"
libraryDependencies += "org.apache.hadoop" % "hadoop-aws" % "2.7.3"
libraryDependencies += "com.amazonaws" % "aws-java-sdk" % "1.7.4"

libraryDependencies += "com.google.protobuf" % "protobuf-java" % "3.11.1"
libraryDependencies += "com.google.protobuf" % "protobuf-java-util" % "3.11.1"

libraryDependencies += "org.mongodb.spark" %% "mongo-spark-connector" % "2.4.1"

// libraryDependencies += "com.thesamet.scalapb" %% "scalapb-runtime" % scalapbVersion % "protobuf"
//libraryDependencies += "com.thesamet.scalapb" %% "scalapb-runtime" % "0.9.3"

// libraryDependencies += "com.thesamet.scalapb" %% "scalapb-json4s" % "0.9.3"
// libraryDependencies += "com.amazonaws" % "aws-java-sdk-core" % "1.11.699"
// libraryDependencies += "com.amazonaws" % "aws-java-sdk-s3" % "1.11.699"

// Compile / unmanagedJars ++= {
//   val base = file("/repo/spark/jars")
//   val customJars = (base ** "*.jar")
//   customJars.classpath
// }
