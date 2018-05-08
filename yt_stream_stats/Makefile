all : yt_stream_stats

CNTOOLS:= \
	../cntools/ssl/cnhttpclient.c \
	../cntools/ssl/cnhttpcommon.c \
	../cntools/ssl/cnsslclient.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -I../cntools/ssl -I../jsmn

yt_stream_stats : yt_stream_stats.c $(CNTOOLS) $(JSMN)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean :
	rm -rf *.o *~ chatmon


