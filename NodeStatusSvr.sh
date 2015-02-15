#* * * * *  /etc/rc.d/init.d/Imgo.NodeMonitor.s
#every minute 
#!/bin/bash
set -x

source /etc/profile

process=( 'NodeStatusSvr' )

for p in ${process[@]}; do
	item=`ps -ef | grep $p | grep -v "grep"`
	if [ "${#item}" -gt 0 ];then
		echo ""		 
	else	
		nohup NodeStatusSvr &
		echo ""
	fi
done
