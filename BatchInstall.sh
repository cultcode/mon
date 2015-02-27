#!/bin/bash
usage () {
  echo "Usage: $0 USER_PASSWORD ROOT_PASSWORD"
}

if [ "$#" -ne 2 ];then
  usage;
  exit 1;
fi;

password_user=$1
password_su=$2

for ip in $(cat ./ip.txt | sed '/^\s*\$/d')
do
  CMD="\
cd && \
rm -rf ./NodeStatusSvr.install && \
wget -nH -P . -N --no-check-certificate http://github.com/cultcode/mon/raw/master/NodeStatusSvr.install && \
chmod +x ./NodeStatusSvr.install && \
./NodeStatusSvr.install $password_su|& tee ./NodeStatusSvr.install.log\
"
echo $CMD
  expect -c "
    set timeout -1
    spawn ssh -o StrictHostKeyChecking=no luowei@$ip \"$CMD\"
    expect *assword:
    send $password_user\r
    expect eof
    "
done
