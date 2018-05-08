all : yt_oauth_helper

CNTOOLS:= \
	../cntools/ssl/cnhttpclient.c \
	../cntools/ssl/cnsslclient.c \
	../cntools/http/mfs.c \
	../cntools/http/sha1.c \
	../cntools/http/cnhttp.c \
	../cntools/http/http_bsd.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -I../cntools/ssl -I../jsmn -I../cntools/http -DMAX_HTTP_PATHLEN=500

yt_oauth_helper : yt_oauth_helper.c $(CNTOOLS) $(JSMN)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean :
	rm -rf *.o *~ chatmon


