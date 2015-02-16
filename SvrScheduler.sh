#!/bin/bash
#set -x

usage () {
  echo "Usage: $0 SERVICE_NAME < start | stop | restart | status >"
}

if [ "$#" -ne 2 ];then
  usage;
  exit 1;
fi;

SERVICE_DIR=$(dirname "$1")
SERVICE_NAME=$(basename "$1")
COMMAND=$2
RETVAL=0
items=""
pids=""

init () {
  items=`ps -ef | grep -v $0 | grep $SERVICE_NAME | grep -v "grep"`
  pids=`echo -n $items | awk '{print $2}'`

  if [ "${#items}" -gt 0 ];then
    echo $items
  fi;
}

start() {
  init;
  echo "Starting $SERVICE_NAME......"
  cmd_start=""

  if [ "${#items}" -gt 0 ];then
    echo "$SERVICE_NAME already run"
    RETVAL=$?
    return $RETVAL;
  else
    cmd_start="${SERVICE_DIR}/$SERVICE_NAME ${SERVICE_DIR}/${SERVICE_NAME}.config >& ${SERVICE_DIR}/${SERVICE_NAME}.log &"
    echo ${cmd_start}
    echo ${cmd_start}|awk '{system($0)}'
    RETVAL=$?
  fi;

  if [ $RETVAL -eq 0 ];then
    echo "Start Succeed";
    init;
  else 
    echo "Start Fail";
  fi;      		    

  return $RETVAL;
}
 
stop () {
  init;
  echo "Stopping $SERVICE_NAME......" 
  cmd_stop=""

  if [ "${#items}" -gt 0 ];then
    for p in ${pids[@]}; do
      cmd_stop="$cmd_stop""kill -9 $p "
    done
    echo ${cmd_stop}
    echo ${cmd_stop}|awk '{system($0)}'
    RETVAL=$?
  else
    echo "$SERVICE_NAME doesn't run"
    RETVAL=$?
    return $RETVAL;
  fi;

  if [ $RETVAL -eq 0 ]; then        
    echo "Stop Succeed";
  else        
    echo "Stop Fail";
    init;
  fi;

  return $RETVAL;
}
 
status() {
  init;

  if [ "${#items}" -gt 0 ];then
    echo "$SERVICE_NAME already run"
  else	
    echo "$SERVICE_NAME doesn't run"
  fi;

  RETVAL=$?    
  return $RETVAL;
}

case "$COMMAND" in
  start) 
    start;
    RETVAL=$?;
    ;;
  stop)
    stop;
    RETVAL=$?;
    ;;
  restart|reload)
    stop;
    start;
    RETVAL=$?;
    ;;
  status)
    status;
    RETVAL=$?;
    ;;
  *)
    usage;
    ;;
esac;

exit $RETVAL;
