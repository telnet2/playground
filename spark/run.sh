#!/bin/bash

sbt package

/repo/spark/bin/spark-submit \
  --class "demo.LineCounter" \
  --master local[4] \
  target/scala-2.11/line-counter_2.11-1.0.jar