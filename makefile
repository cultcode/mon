CFLAGS=-Wall

SRCS=common.c OperateXml.c SocketHttp.c InitNodeStatus.c GetNodeStatusList.c NodeResourceStatus.c ReportNodeStatus.c main.c
HEADERS=common.h OperateXml.h SocketHttp.h InitNodeStatus.h GetNodeStatusList.h NodeResourceStatus.h ReportNodeStatus.h main.h

LIB_ENDECTT=libendectt.so
LIB_CJSON=libcjson.so

TARGET=NodeStatusSvr

$(TARGET):$(SRCS) $(HEADERS) $(LIB_ENDECTT) $(LIB_CJSON) openssl
	gcc -o $@ $(CFLAGS) $(SRCS) -I./openssl/include -I/usr/include/libxml2 -L./openssl -L. -lcrypto -lm -lxml2 -lendectt -lcjson

$(LIB_ENDECTT):Security.c Security.h openssl
	gcc -shared -fPIC -o $@ $(CFLAGS) -DTOLIBRARY EndecTt.c -I./openssl/include -L./openssl -lcrypto -lm

$(LIB_CJSON):cJSON.c cJSON.h openssl
	gcc -shared -fPIC -o $@ $(CFLAGS) cJSON.c    -I./openssl/include -L./openssl -lcrypto -lm

openssl:
	tar -xzvf openssl-1.0.2.tar.gz && ln -s openssl-1.0.2 openssl && cd openssl && ./config && make

clean:
	rm -rf ./$(LIB_ENDECTT)
	rm -rf ./$(LIB_CJSON)
	rm -rf ./$(TARGET)

install:
	mv -f $(LIB_ENDECTT) /usr/lib64/
	mv -f $(LIB_CJSON) 	/usr/lib64/
