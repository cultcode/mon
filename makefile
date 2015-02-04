CFLAGS=-Wall -g3
IDIR=./openssl/include
LDIR=./openssl
LFLAGS=-lcrypto -lm

all:common.c Security.c SocketHttp.c InitNodeStatus.c GetNodeStatusList.c NodeResourceStatus.c ReportNodeStatus.c main.c
	gcc $(CFLAGS) $^ -I$(IDIR) -L$(LDIR) $(LFLAGS) 
