all : ytchatmon

CNTOOLS:= \
	../cntools/ssl/cnhttpclient.c \
	../cntools/ssl/cnhttpcommon.c \
	../cntools/ssl/cnsslclient.c

JSMN:=../jsmn/jsmn.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -I../cntools/ssl -I../jsmn

ytchatmon : ytchatmon.c $(CNTOOLS) $(JSMN)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS) -DBUILD_EXE -O0 -g

clean :
	rm -rf *.o *~ ytchatmon


