#!/bin/bash

set -x

mkdir -p ${BACKUP_DIR}

mongodump --uri ${PROD_DATABASE_URL} -o ${BACKUP_DIR}

if [[ -d ${BACKUP_DIR}/xraydb ]]; then
    mongorestore --uri ${TEST_DATABASE_URL} --db {{dbName}} --drop ${BACKUP_DIR}/{{collection}}
fi
