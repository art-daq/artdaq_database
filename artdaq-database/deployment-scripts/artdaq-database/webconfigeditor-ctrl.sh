#!/bin/bash

rc_success=0 
rc_failure=1

#source /grid/fermiapp/products/artdaq/setup
#source /cvmfs/fermilab.opensciencegrid.org/products/artdaq/setup

unset http_proxy
unset https_proxy

if [ -z ${MONGOD_DATABASE_NAME+x} ]; then
  echo "Error: MONGOD_DATABASE_NAME is unset. Aborting."; exit $rc_failure;  else
  echo "Info: MONGOD_DATABASE_NAME is set to '$MONGOD_DATABASE_NAME'";
fi

if [ -z ${MONGOD_BASE_DIR+x} ]; then
  echo "Error: MONGOD_BASE_DIR is unset. Aborting."; exit $rc_failure;  else
  echo "Info: MONGOD_BASE_DIR is set to '$MONGOD_BASE_DIR'";
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

if [ -z ${WEBEDITOR_UPS_QUAL+x} ]; then
  echo "Error: WEBEDITOR_UPS_QUAL is unset. Aborting.";exit $rc_failure;  else
  echo "Info: WEBEDITOR_UPS_QUAL is set to '$WEBEDITOR_UPS_QUAL'";
fi

if [ -z ${MONGOD_PORT+x} ]; then
  echo "Error: MONGOD_PORT is unset. Aborting.";exit $rc_failure;  else
  echo "Info: MONGOD_PORT is set to '$MONGOD_PORT'";
fi

if ! [[ ${MONGOD_PORT} == ?(-)+([0-9]) ]]; then
  echo "Error: MONGOD_PORT is not a number"; exit $rc_failure;
fi

if [ -z ${WEBEDITOR_BASE_PORT+x} ]; then
  echo "Error: WEBEDITOR_BASE_PORT is unset. Aborting.";exit $rc_failure;  else
  echo "Info: WEBEDITOR_BASE_PORT is set to '$WEBEDITOR_BASE_PORT'";
fi

if ! [[ ${WEBEDITOR_BASE_PORT} == ?(-)+([0-9]) ]]; then
  echo "Error: WEBEDITOR_BASE_PORT is not a number"; exit $rc_failure;
fi

source /daq/artdaq/products/setup
#source /daq/database/initd_functions
unsetup_all  >/dev/null 2>&1 
setup artdaq_node_server ${WEBEDITOR_UPS_VER} -q ${WEBEDITOR_UPS_QUAL} 
RC=$?
if [ $RC -ne 0 ]; then
  echo "Error: Failed setting artdaq_node_server. Aborting. "; exit $rc_failure;
fi

node_bin=$(command -v node)
if [ -z "$node_bin" ] &&  [ ! -x ${node_bin} ]; then
  echo "Error: node was not setup. Aborting."; exit $rc_failure; else
  echo "Info: node found: '${node_bin}'";
fi

WEBEDITOR_PID=${MONGOD_BASE_DIR}/${MONGOD_DATABASE_NAME}/var/tmp/webconfigeditor.pid

WEBEDITOR_DIR=$(dirname ${WEBEDITOR_PID})

if [ ! -d ${WEBEDITOR_DIR} ]; then
  mkdir -p ${WEBEDITOR_DIR} >/dev/null 2>&1
fi

WEBEDITOR_DATA_DIR=$(dirname ${WEBEDITOR_ENV_FILE})/webconfigeditor

if [ ! -d ${WEBEDITOR_DATA_DIR} ]; then
  mkdir -p ${WEBEDITOR_DATA_DIR} >/dev/null 2>&1
fi

WEBEDITOR_LOGS_DIR=$(dirname ${WEBEDITOR_ENV_FILE})/logs

if [ ! -d ${MONGOD_LOGS_DIR} ]; then
  mkdir -p ${MONGOD_LOGS_DIR} >/dev/null 2>&1
fi

WEBEDITOR_LOG=${WEBEDITOR_LOGS_DIR}/webconfigeditor-$(date -d "today" +"%Y%m%d%H%M").log

echo "Info: logfile=${WEBEDITOR_LOG}"

WEBEDITOR_LOCK=${WEBEDITOR_DIR}/webconfigeditor.lock

source /daq/database/initd_functions

start()
{
  #  ulimit -f unlimited
  #  ulimit -t unlimited
  #  ulimit -v unlimited
  #  ulimit -n 64000
  #  ulimit -m unlimited
  #  ulimit -u 64000

  echo -n $"Starting Web Config Editor: "

  export ARTDAQ_DATABASE_URI="mongodb://127.0.0.1:${MONGOD_PORT}/cern_pddaq_db"
  export ARTDAQ_DATABASE_DIR="${WEBEDITOR_DATA_DIR}"

  if [ ! -d "${ARTDAQ_DATABASE_DATADIR}" ]; then
    mkdir -p ${ARTDAQ_DATABASE_DATADIR}
  else
    # rm -rf ${ARTDAQ_DATABASE_DATADIR}
    mkdir -p ${ARTDAQ_DATABASE_DATADIR}
    mkdir -p ${ARTDAQ_DATABASE_DATADIR}/TRASH
    mkdir -p ${ARTDAQ_DATABASE_DATADIR}/tmp
    mkdir -p ${ARTDAQ_DATABASE_DATADIR}/db
  fi
  echo "Starting web gui server $(date)">> ${WEBEDITOR_LOG}  2>&1
  printenv  >> ${WEBEDITOR_LOG}  2>&1
  ldd  $ARTDAQ_DATABASE_FQ_DIR/lib/node_modules/conftoolg/conftoolg.node  >> ${WEBEDITOR_LOG}  2>&1
  echo "ARTDAQ_DATABASE_URI=$ARTDAQ_DATABASE_URI" >> ${WEBEDITOR_LOG}  2>&1
  echo "nodejs=$node_bin" >> ${WEBEDITOR_LOG}  2>&1
  nodejs_app="${ARTDAQ_NODE_SERVER_DIR}/serverbase.js"
  echo "nodejs_app=$nodejs_app" >> ${WEBEDITOR_LOG}  2>&1
  echo "Starting nodejs application -> $nodejs_app"
  cd $(dirname ${nodejs_app})

  nohup $node_bin $(basename ${nodejs_app}) >> ${WEBEDITOR_LOG}  2>&1 &
  RETVAL=$?
  echo $! >${WEBEDITOR_PID}
  echo
  [ $RETVAL -eq 0 ] && touch ${WEBEDITOR_LOCK}
}

stop()
{
  echo -n $"Stopping Web Config Editor: "
  _killproc ${WEBEDITOR_PID} ${node_bin}
  RETVAL=$?
  echo
  [ $RETVAL -eq 0 ] && rm -f ${WEBEDITOR_LOCK}
}

restart () {
  stop
  killall node
  start
}

_killproc()
{
  local pid_file=$1
  local procname=$2
  local -i delay=300
  local -i duration=10
  local pid=`pidofproc -p "${pid_file}" ${procname}`

  kill -TERM $pid >/dev/null 2>&1
  usleep 100000
  local -i x=0
  while [ $x -le $delay ] && checkpid $pid; do
    sleep $duration
    x=$(( $x + $duration))
  done

  kill -KILL $pid >/dev/null 2>&1
  usleep 100000

  checkpid $pid # returns 0 only if the process exists
  local RC=$?
  [ "$RC" -eq 0 ] && failure "${procname} shutdown" || rm -f "${pid_file}"; success "${procname} shutdown"
  RC=$((! $RC)) # invert return code so we return 0 when process is dead.
  return $RC
}


RETVAL=0

case "$1" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  restart|reload|force-reload)
    restart
    ;;
  condrestart)
    [ -f ${WEBEDITOR_LOCK} ] && restart || :
    ;;
  keeprunning)
    curl http://localhost:${WEBEDITOR_BASE_PORT}/db/client.html -s -f -o /dev/null
    RETVAL=$?
    if [ "$RETVAL" -ne 0 ];then 
      echo "Web Config Editor is not responding $RETVAL"
      restart
    fi
    ;;
  status)
    status -p $WEBEDITOR_PID ${node_bin}
    RETVAL=$?

    if [ "$RETVAL" -eq 0 ]; then
      echo Testing URL=http://localhost:${WEBEDITOR_BASE_PORT}/db/client.html 
      #curl http://localhost:${WEBEDITOR_BASE_PORT}/db/client.html 
      curl http://localhost:${WEBEDITOR_BASE_PORT}/db/client.html -s -f -o /dev/null
      RETVAL=$?
      if [ "$RETVAL" -eq 0 ]; then 
        echo Web Config Editor is running
        success "Web Config Editor is running"; else
        echo Web Config Editor is  not running
        failure "Web Config Editor is not running" 
      fi
    fi
    ;;
  *)
    echo "Usage: $0 {start|stop|status|restart|condrestart|keeprunning}"
    RETVAL=1
esac

exit $RETVAL

