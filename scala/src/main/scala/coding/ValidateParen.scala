package coding

object ValidateParen extends App {
    override def main(args: Array[String]): Unit = {
        val x = "Kim".toList
        x match {
            case Nil => println("empty")
            case head :: tail => println(tail.toString)
        }
    }
}