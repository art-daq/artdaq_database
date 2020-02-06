#!/bin/bash

#crontab
#10 22 * * 0  experiment /data/artdaq_database/backup_artdaq_database.sh <database> >>/data/artdaq_database/database-backup.log 2>&1
#examples:
#10 01 * * * /data/artdaq_database/backup_artdaq_database.sh experiment_v4x_db  >>/data/artdaq_database/database-backup-experiment_v4x_db.log 2>&1
#30 01 * * * /data/artdaq_database/backup_artdaq_database.sh experiment_v4x_db >>/data/artdaq_database/database-backup-experiment_v4x_db.log 2>&1

this_experimentdaq_password="password"
this_admin_password="prefix$this_experimentdaq_password"


rc_success=0 
rc_failure=1

echo "Backup started: $(date)"
echo "Running: $0 $@"

MONGOD_BASE_DIR=$(dirname $(readlink -f "$0") )

MONGOD_DATABASE_NAME=$1

if [ -z ${MONGOD_DATABASE_NAME+x} ]; then
        echo "Error: MONGOD_DATABASE_NAME is unset. Aborting."; exit $rc_failure;  else
        echo "Info: MONGOD_DATABASE_NAME is set to '$MONGOD_DATABASE_NAME'";
fi

if [ -z ${MONGOD_BASE_DIR+x} ]; then
        echo "Error: MONGOD_BASE_DIR is unset. Aborting."; exit $rc_failure;  else
        echo "Info: MONGOD_BASE_DIR is set to '$MONGOD_BASE_DIR'";
fi

MONGOD_ENV_FILE=${MONGOD_BASE_DIR}/${MONGOD_DATABASE_NAME}/mongod.env

if [ ! -f ${MONGOD_ENV_FILE} ]; then
        echo "Error: ${MONGOD_ENV_FILE} not found! Aborting."; exit $rc_failure ; else
        source ${MONGOD_ENV_FILE}
fi

WEBEDITOR_ENV_FILE=${MONGOD_BASE_DIR}/${MONGOD_DATABASE_NAME}/webconfigeditor.env

if [ ! -f ${WEBEDITOR_ENV_FILE} ]; then
        echo "Error: ${WEBEDITOR_ENV_FILE} not found! Aborting."; exit $rc_failure ; else
        source ${WEBEDITOR_ENV_FILE}
fi

if [ -z ${WEBEDITOR_UPS_VER+x} ]; then
        echo "Error: WEBEDITOR_UPS_VER is unset. Aborting.";exit $rc_failure;  else
        echo "Info: WEBEDITOR_UPS_VER is set to '$WEBEDITOR_UPS_VER'";
fi

source /software/products/setup

setup artdaq_database v1_04_82 -q e17:prof:s83
RC=$?
if [ $RC -ne 0 ]; then
        echo "Error: Failed setting artdaq_database. Aborting. "; exit $rc_failure;
fi

ups active |grep artdaq_database

conftool_bin=$(command -v conftool.py)
if [ -z "$node_bin" ] &&  [ ! -x ${node_bin} ]; then
        echo "Error: conftool.py was not setup. Aborting."; exit $rc_failure; else
        echo "Info: conftool.py found: '${conftool_bin}'";
fi


DBBACKUP_DIR="${MONGOD_BASE_DIR}/${MONGOD_DATABASE_NAME}/backup/$(date +%Y%m%d%H%M%S)"
DBBACKUP_LIST="experiment_db experiment_db_archive"

#exit $rc_failure
unset http_proxy
unset https_proxy

echo "DATABASE_NAME is ${MONGOD_DATABASE_NAME}"
for dbname in ${DBBACKUP_LIST};
do
  export ARTDAQ_DATABASE_URI="mongodb://admin:$this_admin_password@192.168.1.1:28047,192.168.1.2:28047/${dbname}?replicaSet=rs0&authSource=admin"
  #export ARTDAQ_DATABASE_URI="mongodb://127.0.0.1:${MONGOD_PORT}/${dbname}";
  mkdir -p ${DBBACKUP_DIR}/${dbname}  
  cd $DBBACKUP_DIR/$dbname  
  echo "Backing up ${ARTDAQ_DATABASE_URI} to ${DBBACKUP_DIR}/${dbname}"
  conftool.py exportDatabase
  mongodump --uri=$ARTDAQ_DATABASE_URI --gzip
done

[[ ! -d /software/backup/experiment_v4x_db ]] && mkdir -p /software/backup/experiment_v4x_db

rsync -av /data/artdaq_database/experiment_v4x_db/backup /software/backup/experiment_v4x_db/
echo  "Backup ended: $(date)"
echo

