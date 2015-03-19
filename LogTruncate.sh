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

SERVICE_DIR=$(dirname "$1")
SERVICE_NAME=$(basename "$1")

LOG_FILE=$2
LOG_FILE_BAK=${LOG_FILE}"."$(date +"%Y%m%d_%H%M%S")

RETVAL=0
items=""
pids=""
length=0
IFS="!!"

init () {
  items=(`ps -ef | grep "[0-9]\+:[0-9]\+:[0-9]\+ $SERVICE_DIR/$SERVICE_NAME\b"`)
  length=${#items[@]}

  if [ "$length" -gt 0 ];then
    for ((i=0; i<$length; i++))
    do
      pids[$i]=`echo ${items[$i]} | awk '{print $2}'`
      echo ${items[$i]}
      echo ${pids[$i]}
    done
  fi;

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
