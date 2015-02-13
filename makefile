CFLAGS=-Wall -g
IDIR=-I./openssl/include -I/usr/include/libxml2
LDIR=-L./openssl
LFLAGS=-lcrypto -lm

all:cJSON.c common.c Security.c SocketHttp.c InitNodeStatus.c GetNodeStatusList.c NodeResourceStatus.c ReportNodeStatus.c main.c
	gcc -o NodeStatusSvr $(CFLAGS) $^ $(IDIR) $(LDIR) $(LFLAGS) 
