Basic:
  NodeStatusSvr         :executable file
  NodeStatusSvr.config  :contains run-time arguments, please run NodeStatusSvr -h to get more info.
  NodeStatusSvr.sh      :shell script used to start up NodeStatusSvr when it is not in running.

How to install:
  1. put all 3 files above into any directory you want, like YOUR_DIR
  2. add below line into cron job list:
    * * * * * YOU_DIR/NodeStatusSvr.sh >/dev/null 2>&1
  3. check if any error emerged in YOU_DIR/NodeStatusSvr.sh.log
