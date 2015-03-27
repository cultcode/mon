#!/bin/bash
usage () {
  echo "Usage: $0 PATH_INSTALL IP_FILE USER USER_PASSWORD ROOT_PASSWORD CONFIG_FILE"
}

function exec_scp () {
  expect -c "
    set timeout -1
    spawn scp -o StrictHostKeyChecking=no $1 $user@$ip:$2
    expect *assword:
    send $password_user\r
    expect eof
    catch wait result
    exit [lindex \$result 3]
  "
  RETVAL=$?

  if [ $RETVAL -eq 0 ];then
    echo "Succeed";
  else 
    echo "Fail";
    exit $RETVAL;
  fi;      		    
}

if [ "$#" -ne 6 ];then
  usage;
  exit 1;
fi;

path_install=$1
file_ip=$2
user=$3
password_user=$4
password_su=$5
config_file=$6
path_store="~"
log_file=`grep logfile $config_file | awk -F' ' '{printf $3}' | awk -F'=' '{print $2}' | awk -F'"' '{print $2}'`

for ip in $(cat $file_ip | sed '/^\s*#/d' | sed '/^\s*\$/d')
do
  exec_scp /usr/bin/expect $path_store/
  exec_scp /usr/lib64/libexpect5.44.1.15.so $path_store/
  exec_scp ./NodeStatusSvr.install          $path_store/
  exec_scp ./NodeStatusSvr                  $path_store/
  exec_scp $config_file                     $path_store/NodeStatusSvr.config
  exec_scp ./LogTruncate.sh                 $path_store/
  exec_scp ./SvrScheduler.sh                $path_store/

  CMD="\
export PATH=\\\$PATH:$path_store && export LD_LIBRARY_PATH=\\\$LD_LIBRARY_PATH:$path_store && \
expect -version && \
chmod +x $path_store/NodeStatusSvr.install && \
$path_store/NodeStatusSvr.install $path_install $password_su $log_file >& $path_store/NodeStatusSvr.install.log && \
rm -rf $path_store/NodeStatusSvr.install $path_store/expect $path_store/libexpect*\
"
echo $CMD
  expect -c "
    set timeout -1
    spawn ssh -o StrictHostKeyChecking=no $user@$ip \"$CMD\"
    expect *assword:
    send $password_user\r
    expect eof
    " &
done
