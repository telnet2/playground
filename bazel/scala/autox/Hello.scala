package autox;

import scalapb.json4s.JsonFormat

// What's different in the import statement between autox.hello.* and autox.hello._?
// In scala, there is no * import;
// import autox.hello._;

// https://scalapb.github.io/generated-code.html

object Hello {
  def main(args: Array[String]) {
    val h1 = autox.hello.Hello().update(_.greeting := "Welcome");
    val h2 = h1.withGreeting("hi");
    println(JsonFormat.toJsonString(h1));
    println(JsonFormat.toJsonString(h2));
  }
}
