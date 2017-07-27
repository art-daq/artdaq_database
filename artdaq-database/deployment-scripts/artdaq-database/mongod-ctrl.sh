#!/bin/bash

rc_success=0 
rc_failure=1

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

if [ -z ${MONGOD_UPS_VER+x} ]; then
        echo "Error: MONGOD_UPS_VER is unset. Aborting.";exit $rc_failure;  else
        echo "Info: MONGOD_UPS_VER is set to '$MONGOD_UPS_VER'";
fi

if [ -z ${MONGOD_UPS_QUAL+x} ]; then
        echo "Error: MONGOD_UPS_QUAL is unset. Aborting.";exit $rc_failure;  else
        echo "Info: MONGOD_UPS_QUAL is set to '$MONGOD_UPS_QUAL'";
fi

if [ -z ${MONGOD_PORT+x} ]; then
        echo "Error: MONGOD_PORT is unset. Aborting.";exit $rc_failure;  else
        echo "Info: MONGOD_PORT is set to '$MONGOD_PORT'";
fi

if ! [[ ${MONGOD_PORT} == ?(-)+([0-9]) ]]; then
	echo "Error: MONGOD_PORT is Not a number"; exit $rc_failure;
fi


source /daq/artdaq/products/setup
source /daq/database/initd_functions
unsetup_all  >/dev/null 2>&1 
setup mongodb ${MONGOD_UPS_VER} -q ${MONGOD_UPS_QUAL} 
RC=$?
if [ $RC -ne 0 ]; then
	echo "Error: Failed setting mongodb. Aborting. "; exit $rc_failure;
fi

mongod_bin=$(command -v mongod)
if [ -z "$mongo_bin" ] &&  [ ! -x ${mongod_bin} ]; then
	echo "Error: mongodb was not setup. Aborting."; exit $rc_failure; else
        echo "Info: mongod found: '${mongod_bin}'";
fi

MONGOD_PID=${MONGOD_BASE_DIR}/${MONGOD_DATABASE_NAME}/var/tmp/mongod.pid

MONGOD_DIR=$(dirname ${MONGOD_PID})

if [ ! -d ${MONGOD_DIR} ]; then
	mkdir -p ${MONGOD_DIR} >/dev/null 2>&1
fi

MONGOD_DATA_DIR=$(dirname ${MONGOD_ENV_FILE})/data

if [ ! -d ${MONGOD_DATA_DIR} ]; then
        mkdir -p ${MONGOD_DATA_DIR} >/dev/null 2>&1
fi

MONGOD_LOGS_DIR=$(dirname ${MONGOD_ENV_FILE})/logs

if [ ! -d ${MONGOD_LOGS_DIR} ]; then
        mkdir -p ${MONGOD_LOGS_DIR} >/dev/null 2>&1
fi

MONGOD_LOG=${MONGOD_LOGS_DIR}/mongod-$(date -d "today" +"%Y%m%d%H%M").log

echo "Info: logfile=${MONGOD_LOG}"

MONGOD_LOCK=${MONGOD_DIR}/mongod.lock


start()
{
#  ulimit -f unlimited
#  ulimit -t unlimited
#  ulimit -v unlimited
#  ulimit -n 64000
#  ulimit -m unlimited
#  ulimit -u 64000

  echo -n $"Starting mongod: "

  ${MONGOD_NUMA_CTRL} ${mongod_bin} \
	--dbpath=${MONGOD_DATA_DIR} \
 	--pidfilepath=${MONGOD_PID} \
	--port=${MONGOD_PORT} \
	--bind_ip=127.0.0.1 \
	--logpath=${MONGOD_LOG} \
	--logappend \
	--fork # >/dev/null 2>&1

  RETVAL=$?
  echo
  [ $RETVAL -eq 0 ] && touch ${MONGOD_LOCK}
}

stop()
{
  echo -n $"Stopping mongod: "
  _killproc ${MONGOD_PID} ${mongod_bin}
  RETVAL=$?
  echo
  [ $RETVAL -eq 0 ] && rm -f ${MONGOD_LOCK}
}

restart () {
        stop
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
    [ -f ${MONGOD_LOCK} ] && restart || :
    ;;
  status)
    status -p ${MONGOD_PID} ${mongod_bin}
    RETVAL=$?
    ;;
  *)
    echo "Usage: $0 {start|stop|status|restart|condrestart}"
    RETVAL=1
esac

exit $RETVAL

