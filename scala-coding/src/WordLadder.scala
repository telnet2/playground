object WordLadder extends App {

    implicit class WordRelation(s: String) {
        def adjacent(v: String): Boolean = {
            if (s.length != v.length) {
                false
            } else {
                s.length == s.zip(v).map(x => if (x._1 == x._2) 1 else 0).sum + 1
            }
        }
    }

    println("hot".adjacent("hat")) // true
    println("hot".adjacent("dat")) // false
}
