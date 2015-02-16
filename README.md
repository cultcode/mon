Files:
  1.NodeStatusSvr         :executable file 
  2.NodeStatusSvr.config  :contains run-time arguments, please run NodeStatusSvr -h to get more info. 
  3.SvrScheduler.sh       :shell script used to start/restart/stop NodeStatusSvr

How to install:
  1. add below line into cron job list:
    * * * * * SvrScheduler.sh NodeStatusSvr start >/dev/null 2>&1
