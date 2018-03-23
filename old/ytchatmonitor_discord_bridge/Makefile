all : discordbridge

CNTOOLS:= \
	cntools/ssl/cnhttpclient.c \
	cntools/ssl/cnhttpcommon.c \
	cntools/ssl/cnsslclient.c

JSMN:=jsmn/jsmn.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -Icntools/ssl -Ijsmn

discordbridge : discordbridge.c $(CNTOOLS) $(JSMN)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean :
	rm -rf *.o *~ discordbridge


