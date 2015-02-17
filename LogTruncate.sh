#!/bin/bash
#set -x

usage () {
  echo "Usage: $0 SERVICE_NAME LOG_FILE"
}

if [ "$#" -ne 2 ];then
  usage;
  exit 1;
fi;

if [ ! -f "$1" ];then
  echo "SERVICE_NAME $1 is not existent"
  usage;
  exit 1;
fi;

if [ ! -f "$2" ];then
  echo "LOG_FILE $2 is not existent"
  usage;
  exit 1;
fi;

SERVICE_NAME=$1

LOG_FILE=$2
LOG_FILE_BAK=${LOG_FILE}"."$(date +"%Y%m%d_%H%M%S")

RETVAL=0
items=""
pids=""

init () {
  items=`ps -ef | grep -v $0 | grep $SERVICE_NAME | grep -v "grep"`
  pids=`echo -n $items | awk '{print $2}'`

  mv $LOG_FILE $LOG_FILE_BAK
  RETVAL=$?
  if [ $RETVAL -eq 0 ];then
    echo "Mv $LOG_FILE $LOG_FILE_BAK Succeed";
  else 
    echo "Mv $LOG_FILE $LOG_FILE_BAK Fail";
  fi;      		    

  if [ "${#items}" -le 0 ];then
    echo "$SERVICE_NAME doesn't run"
    return 1;
  fi;

  for p in ${pids[@]}; do
    kill -USR1 $p
    RETVAL=$?

    if [ $RETVAL -eq 0 ];then
      echo "Truncate $LOG_FILE of Process $p Succeed";
    else 
      echo "Truncate $LOG_FILE of Process $p Fail";
    fi;      		    

  done
  return $RETVAL;
}

init;
RETVAL=$?
exit $RETVAL;
