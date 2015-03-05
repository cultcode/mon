CFLAGS=-Wall

SRCS=common.c OperateXml.c SocketHttp.c InitNodeStatus.c GetNodeStatusList.c ReportNodeStatus.c main.c
HEADERS=common.h OperateXml.h SocketHttp.h InitNodeStatus.h GetNodeStatusList.h ReportNodeStatus.h main.h

LIB_ENDECTT=libendectt.so
LIB_CJSON=libcjson.so
LIB_GNRS=libgnrs.so

TARGET=NodeStatusSvr

$(TARGET):$(SRCS) $(HEADERS) $(LIB_ENDECTT) $(LIB_CJSON) $(LIB_GNRS) openssl
	gcc -o $@ $(CFLAGS) $(SRCS) -I./openssl/include -I/usr/include/libxml2 -L./openssl -L. -lcrypto -lm -lxml2 -lendectt -lcjson -lgnrs

$(LIB_ENDECTT):Security.c Security.h openssl
	gcc -shared -fPIC -o $@ $(CFLAGS) -DTOLIBRARY Security.c -I./openssl/include -L./openssl -lcrypto -lm

$(LIB_CJSON):cJSON.c cJSON.h
	gcc -shared -fPIC -o $@ $(CFLAGS) cJSON.c

$(LIB_GNRS):$(LIB_ENDECTT) common.c common.h SocketHttp.c SocketHttp.h NodeResourceStatus.c NodeResourceStatus.h GetNodeResourceStatus.c GetNodeResourceStatus.h
	gcc -shared -fPIC -o $@ $(CFLAGS) common.c SocketHttp.c NodeResourceStatus.c GetNodeResourceStatus.c -L. -lendectt

openssl:
	tar -xzvf openssl-1.0.2.tar.gz && ln -s openssl-1.0.2 openssl && cd openssl && ./config && make

clean:
	rm -rf ./$(TARGET)
	rm -rf ./$(LIB_ENDECTT)
	rm -rf ./$(LIB_CJSON)
	rm -rf ./$(LIB_GNRS)

install:
	cp -f $(LIB_ENDECTT) /usr/lib64/
	cp -f $(LIB_CJSON) 	 /usr/lib64/
	cp -f $(LIB_GNRS) 	 /usr/lib64/
