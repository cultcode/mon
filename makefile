CFLAGS=-Wall -g
IDIR=./openssl/include
LDIR=./openssl
LFLAGS=-lcrypto -lm

all:cJSON.c common.c Security.c SocketHttp.c InitNodeStatus.c GetNodeStatusList.c NodeResourceStatus.c ReportNodeStatus.c main.c
	gcc -o NodeStatusSvr $(CFLAGS) $^ -I$(IDIR) -L$(LDIR) $(LFLAGS) 
