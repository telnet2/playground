package mapping

/* LineCounter.scala */
// import org.apache.spark._
import java.time.format.DateTimeFormatter

import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.functions._
// import org.apache.spark.sql.SparkSession

object MetadataMongoImport {
  def main(params: Array[String]) {
//    val decoder = Base64.getDecoder()
    val spark = SparkSession.builder
      .appName("MetadataMongoImporter")
      .config("spark.master", "local")
      .config(
        "spark.mongodb.output.uri",
        "mongodb://autox:xIiwic3ViIjoibGR@lxmap.autox.sh:8470/lidardb-test.mapping_metadata_import"
      )
      .getOrCreate()

    val baseUrl =
      "s3a://autox-mapping/metadata/2019_12_31_13_52_30_map_update.meta"

    val logData = spark.read
      .option("header", "false")
      .option("delimiter", " ")
      .csv(s"$baseUrl/part-00000")
      .toDF("frameId", "encoded")

//    logData.withColumn("encoded", unbase64(col("encoded")))

    val postfix = DateTimeFormatter
      .ofPattern("YYYY_MM_dd_HH_mm_ss")
      .format(java.time.LocalDateTime.now)
//    val outputDir = s"s3a://autox-mapping/metadata/test.${}"
//    println(outputDir)
//    logData.write.parquet(s"/tmp/test_${postfix}")

    //    def getJson(b64: String) = {
    //      JsonFormat.printer().print(LidarFrame.parseFrom(decoder.decode(b64)))
    //    }

//    println(getJson(logData.head().getAs[String](1)))

//    val docs = logData.map(row => getJson(row.getAs[String](1)))
//    docs.collect.foreach(println)
//    docs.write.format("csv").csv(s"s3a://autox-mapping/test.${outputDir}/metadata")
//    MongoSpark.save(documents)

//    println(firstRow)
//    val proto = LidarFrame.parseFrom(decoder.decode(firstRow.getAs[String](1)))
//    val json = JsonFormat.printer().print(proto)
//    val bson = Document.parse(json)
//    println(bson)

    // logData.filter(row => row.getAs[String](1).contains("a")).count()

    // println(s"Lines with a: $numAs")
    // val numBs = logData.filter(line => line.contains("b")).count()
    // println(s"Lines with a: $numAs, Lines with b: $numBs")
    spark.stop()
  }
}
