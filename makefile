CFLAGS=-Wall -g3
LDFLAGS=

NodeStatusSvr:SocketHttp.c InitNodeStatus.c GetNodeStatusList.c NodeResourceStatus.c ReportNodeStatus.c main.c
	gcc $^ $(CFLAGS) $(LDFLAGS)

