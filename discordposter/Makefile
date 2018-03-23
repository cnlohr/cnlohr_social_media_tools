all : discordposter

CNTOOLS:= \
	../cntools/ssl/cnhttpclient.c \
	../cntools/ssl/cnhttpcommon.c \
	../cntools/ssl/cnsslclient.c

JSMN:=../jsmn/jsmn.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -I../cntools/ssl -I../jsmn

discordposter : discordposter.c $(CNTOOLS) $(JSMN)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean :
	rm -rf *.o *~ chatmon


