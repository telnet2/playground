package coding

import com.github.andyglow.websocket._

object Problems extends App {
  def testMatch(): Unit = {
    val x = "Kim".toList
    x match {
      case Nil          => println("empty")
      case head :: tail => println(tail.toString)
    }
  }

  def scalaWebSocket(): Unit = {
    // 1. prepare ws-client
    // 2. define message handler
    val cli = WebsocketClient[String]("ws://echo.websocket.org") {
      case str =>
        println(s"<<| $str")
    }

    // 4. open websocket
    val ws = cli.open()

    // 5. send messages
    ws ! "hello"
    ws ! "world"
  }

  override def main(args: Array[String]): Unit = {
    scalaWebSocket
  }
}
