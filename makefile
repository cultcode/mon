CFLAGS=-Wall -g3
IDIR=/usr/local/ssl/include
LDIR=/usr/local/ssl/lib
LFLAGS=-lcrypto

NodeStatusSvr:common.c cbc.c SocketHttp.c InitNodeStatus.c GetNodeStatusList.c NodeResourceStatus.c ReportNodeStatus.c main.c
	gcc $^ $(CFLAGS) -I $(IDIR) -L $(LDIR) $(LFLAGS)
