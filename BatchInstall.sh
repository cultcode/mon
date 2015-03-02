#!/bin/bash
usage () {
  echo "Usage: $0 IP_FILE USER USER_PASSWORD ROOT_PASSWORD CONFIG_FILE"
}

function exec_scp () {
  expect -c "
    set timeout -1
    spawn scp -o StrictHostKeyChecking=no $1 $user@$ip:$2
    expect *assword:
    send $password_user\r
    expect eof
  "
}

if [ "$#" -ne 5 ];then
  usage;
  exit 1;
fi;

file_ip=$1
user=$2
password_user=$3
password_su=$4
config_file=$5
path_store="~"

for ip in $(cat $file_ip | sed '/^#/d' | sed '/^\s*\$/d')
do
  exec_scp /usr/bin/expect \~/
  exec_scp /usr/lib64/libexpect5.44.1.15.so \~/
  exec_scp ./NodeStatusSvr.install \~/
  exec_scp ./NodeStatusSvr    \~/
  exec_scp $config_file     \~/NodeStatusSvr.config
  exec_scp ./LogTruncate.sh   \~/
  exec_scp ./SvrScheduler.sh  \~/

  CMD="\
export PATH=\\\$PATH:$path_store && export LD_LIBRARY_PATH=\\\$LD_LIBRARY_PATH:$path_store && \
expect -version && \
chmod +x $path_store/NodeStatusSvr.install && \
$path_store/NodeStatusSvr.install $password_su >& $path_store/NodeStatusSvr.install.log && \
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
