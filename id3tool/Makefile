all : id3socialtool

id3socialtool : id3socialtool.c id3v2lib/src/frame.c id3v2lib/src/header.c id3v2lib/src/id3v2lib.c id3v2lib/src/types.c id3v2lib/src/utils.c
	gcc -o $@ $^ -Iid3v2lib/include -Iid3v2lib/include/id3v2lib -g

clean:
	rm -rf *.o ~* id3socialtool

