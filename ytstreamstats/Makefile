all : ytstreamstats

CNTOOLS:= \
	../cntools/ssl/cnhttpclient.c \
	../cntools/ssl/cnhttpcommon.c \
	../cntools/ssl/cnsslclient.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -I../cntools/ssl -I../jsmn

ytstreamstats : ytstreamstats.c $(CNTOOLS) $(JSMN)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean :
	rm -rf *.o *~ chatmon


