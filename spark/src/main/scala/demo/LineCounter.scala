package mapping

/* LineCounter.scala */
// import org.apache.spark._
import java.util.Base64
import org.apache.spark.sql.SparkSession
import msg.mapreduce.LidarFrame
import scalapb.TextFormat
// import org.apache.spark.sql.SparkSession

object LineCounter {
  def main(args: Array[String]) {
    val decoder = Base64.getDecoder()
    val spark = SparkSession.builder
      .appName("Line Counter")
      .config("spark.master", "local")
      .getOrCreate()

    val baseUrl =
      "s3a://autox-mapping/metadata/2019_12_31_13_52_30_map_update.meta"

    val logData = spark.read
      .option("header", "false")
      .option("delimiter", " ")
      .csv(s"$baseUrl/part-00000")
      .cache()

    val firstRow = logData.head()

    val proto = LidarFrame.parseFrom(decoder.decode(firstRow.getAs[String](1)))
    println(TextFormat.printToString(proto))

    // logData.filter(row => row.getAs[String](1).contains("a")).count()

    // println(s"Lines with a: $numAs")
    // val numBs = logData.filter(line => line.contains("b")).count()
    // println(s"Lines with a: $numAs, Lines with b: $numBs")
    spark.stop()
  }
}
