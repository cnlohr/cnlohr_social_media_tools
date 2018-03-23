all : chatmon

CNTOOLS:= \
	cntools/ssl/cnhttpclient.c \
	cntools/ssl/cnhttpcommon.c \
	cntools/ssl/cnsslclient.c

JSMN:=jsmn/jsmn.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -Icntools/ssl -Ijsmn

chatmon : chatmon.c $(CNTOOLS) $(JSMN)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean :
	rm -rf *.o *~ chatmon


