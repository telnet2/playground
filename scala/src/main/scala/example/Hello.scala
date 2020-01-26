package example

import message.Person
import scalapb.json4s.JsonFormat;

object Hello extends App {
  val p = Person().update(_.name := "Kim")
  println(JsonFormat.toJsonString(p))
}
