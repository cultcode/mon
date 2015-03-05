CFLAGS=-Wall

SRCS=cJSON.c common.c Security.c SocketHttp.c NodeResourceStatus.c GetNodeResourceStatus.c InitNodeStatus.c GetNodeStatusList.c ReportNodeStatus.c OperateXml.c main.c
HEADERS=cJSON.h common.h Security.h SocketHttp.h NodeResourceStatus.h GetNodeResourceStatus.h InitNodeStatus.h GetNodeStatusList.h ReportNodeStatus.h OperateXml.h main.h

LIB_ENDECTT=libendectt.so
LIB_CJSON=libcjson.so
LIB_GNRS=libgnrs.so

TARGET=NodeStatusSvr

$(TARGET):$(SRCS) $(HEADERS) $(LIB_ENDECTT) $(LIB_CJSON) $(LIB_GNRS) openssl
	gcc -o $@ $(CFLAGS) $(SRCS) -I./openssl/include -I/usr/include/libxml2 -L./openssl -lcrypto -lm -lxml2

$(LIB_ENDECTT):Security.c Security.h openssl
	gcc -shared -fPIC -o $@ $(CFLAGS) Security.c -I./openssl/include -L./openssl -lcrypto -lm

$(LIB_CJSON):cJSON.c cJSON.h
	gcc -shared -fPIC -o $@ $(CFLAGS) cJSON.c

$(LIB_GNRS):cJSON.c common.c Security.c SocketHttp.c NodeResourceStatus.c GetNodeResourceStatus.c cJSON.h common.h Security.h SocketHttp.h NodeResourceStatus.h GetNodeResourceStatus.h
	gcc -shared -fPIC -o $@ $(CFLAGS) common.c Security.c SocketHttp.c NodeResourceStatus.c GetNodeResourceStatus.c -I./openssl/include -L./openssl -lcrypto -lm

openssl:
	tar -xzvf openssl-1.0.2.tar.gz && ln -s openssl-1.0.2 openssl && cd openssl && ./config && make

clean:
	rm -rf ./$(TARGET)
	rm -rf ./$(LIB_ENDECTT)
	rm -rf ./$(LIB_CJSON)
	rm -rf ./$(LIB_GNRS)
