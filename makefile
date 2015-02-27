CFLAGS=-Wall -g
IDIR=-I./openssl/include -I/usr/include/libxml2
LDIR=-L./openssl
LFLAGS=-lcrypto -lm -lxml2

SRC=cJSON.c common.c OperateXml.c Security.c SocketHttp.c InitNodeStatus.c GetNodeStatusList.c NodeResourceStatus.c ReportNodeStatus.c main.c

all:openssl $(SRC)
	gcc -o NodeStatusSvr $(CFLAGS) $(SRC) $(IDIR) $(LDIR) $(LFLAGS) 

openssl:
	tar -xzvf openssl-1.0.2.tar.gz && ln -s openssl-1.0.2 openssl && cd openssl && ./config && make
