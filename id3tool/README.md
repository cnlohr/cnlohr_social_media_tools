Tool for reading/writing the id3 comment field in MP3 files.

writeid3source [w/r/Y] [Command]

Wanna freak out your friends with your mad leet command-line foo?

```for f in *.mp3; do echo -ne $f\\t; id3socialtool r "${f}"; echo -ne \\n; done > songlist.csv```

Edit songlist.csv then...

```while read p; do echo -ne $(echo $p| cut -f 2 -d,)\\n$(echo $p|cut -f 3 -d, )\\n | id3socialtool w "$(echo $p|cut -f 1 -d,)"; done < songlist.csv ```

Another way to do it for all subfolders...  

```IFS=$'\n'; for f in `find -iname '*.mp3'`; do id3socialtool Y $f; done > songdb.tsv```

