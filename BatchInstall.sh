#!/bin/bash
usage () {
  echo "Usage: $0 IP_FILE USER USER_PASSWORD ROOT_PASSWORD"
}

if [ "$#" -ne 4 ];then
  usage;
  exit 1;
fi;

file_ip=$1
user=$2
password_user=$3
password_su=$4

for ip in $(cat $file_ip | sed '/^#/d' | sed '/^\s*\$/d')
do
  expect -c "
    set timeout -1
    spawn scp -o StrictHostKeyChecking=no ./expect  $user@$ip:
    expect *assword:
    send $password_user\r
    expect eof
    spawn scp -o StrictHostKeyChecking=no ./libexpect5.44.1.15.so $user@$ip:
    expect *assword:
    send $password_user\r
    expect eof
  "

  CMD="\
cd && \
export PATH=\\\$PATH:. && \
export LD_LIBRARY_PATH=\\\$LD_LIBRARY_PATH:. && \
rm -rf ./NodeStatusSvr.install && \
wget -nH -P . -N --no-check-certificate http://github.com/cultcode/mon/raw/master/NodeStatusSvr.install && \
chmod +x ./NodeStatusSvr.install && \
./NodeStatusSvr.install $password_su|& tee ./NodeStatusSvr.install.log && \
rm -rf ./NodeStatusSvr.install ./expect ./libexpect*\
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
