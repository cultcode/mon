#!/bin/bash
set -x

#source /etc/profile
workdir=$(dirname "$0")
selfname=$(basename "$0")

process=( 'NodeStatusSvr' )

for p in ${process[@]}; do
	item=`ps -ef | grep -v "$0" | grep "$p" | grep -v "grep"`
	if [ "${#item}" -gt 0 ];then
		echo ""		 
	else	
		${workdir}/NodeStatusSvr >& ${workdir}/${selfname}.log &
		echo ""
	fi
done
