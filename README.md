名称：  NodeStatusSvr
功能：  节点资源监控及上报
文件：
NodeStatusSvr：   主程序
NodeStatusSvr.config：  配置文件
SvrScheduler.sh：   运行脚本
LogTruncate.sh:   日志切割脚本


安装步骤：
1. 将以上4个文件拷贝到/home/Imgo.NodeStatusSvr/

2. 添加以下cronjob
* * * * * /home/Imgo.NodeStatusSvr/SvrScheduler.sh /home/Imgo.NodeStatusSvr/NodeStatusSvr start > /dev/null  2>&1
0 0 1 * * /home/Imgo.NodeStatusSvr/LogTruncate.sh  /home/Imgo.NodeStatusSvr/NodeStatusSvr /home/Imgo.NodeStatusSvr/NodeStatusSvr.log > /dev/null  2>&1
