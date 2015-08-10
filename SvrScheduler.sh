#!/bin/bash
#set -x

usage () {
  echo "Usage: $0 SERVICE_NAME < start | stop | restart | status >"
}

if [ "$#" -ne 2 ];then
  usage;
  exit 1;
fi;

#if [ ! -x "$1" ];then
#  echo "$1 is not existent or not executable"
#  exit 1;
#fi;

SERVICE_DIR=$(dirname "$1")
SERVICE_NAME=$(basename "$1")
#IFS=$'\n'

COMMAND=$2
RETVAL=0
items=""
pids=""
length=0

init () {
  IFS_old=$IFS
  IFS=$'\x0A'
  items=(`ps -ef | grep "[0-9]\+:[0-9]\+:[0-9]\+ \S*$SERVICE_NAME\b"`)
  IFS=$IFS_old
  length=${#items[@]}

  if [ "$length" -gt 0 ];then
    for ((i=0; i<$length; i++))
    do
      pids[$i]=`echo ${items[$i]} | awk '{print $2}'`
      echo ${items[$i]}
      echo ${pids[$i]}
    done
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
    cmd_start="nohup ${SERVICE_DIR}/$SERVICE_NAME ${SERVICE_DIR}/${SERVICE_NAME}.config >> ${SERVICE_DIR}/nohup.out 2>&1 &"
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
  cmd_stop="kill -9"

  if [ "${#items}" -gt 0 ];then
    for p in ${pids[@]}; do
      cmd_stop="$cmd_stop"" $p"
    done
    echo ${cmd_stop}
    echo ${cmd_stop}|awk '{system($0)}'
    RETVAL=$?
    sleep 3;
  else
    echo "$SERVICE_NAME doesnot run"
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
    echo "$SERVICE_NAME doesnot run"
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
