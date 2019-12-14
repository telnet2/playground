set -x
mongorestore --uri mongodb://xraydb:0Mm9ZVnMxIiwi@xraydb.autox.sh:8270 --db xraydb --drop $*
