all : ytoauthhelper ytrefreshtoken

CNTOOLS_CLIENT:= \
	../cntools/ssl/cnhttpclient.c \
	../cntools/ssl/cnsslclient.c

CNTOOLS:= \
	../cntools/http/mfs.c \
	../cntools/http/sha1.c \
	../cntools/http/cnhttp.c \
	../cntools/http/http_bsd.c

LDFLAGS:=-lssl -lcrypto
CFLAGS:=-g -I../cntools/ssl -I../cntools -I../cntools/http -DMAX_HTTP_PATHLEN=500

ytoauthhelper : ytoauthhelper.c $(CNTOOLS) $(CNTOOLS_CLIENT)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

ytrefreshtoken : ytrefreshtoken.c $(CNTOOLS_CLIENT) ../cntools/ssl/cnhttpcommon.c
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS) -DMAKE_EXE

clean :
	rm -rf *.o *~ ytrefreshtoken ytrefreshtoken


