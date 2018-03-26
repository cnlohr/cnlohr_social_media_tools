Tool for reading/writing the id3 comment field in MP3 files.

writeid3source [w/r] [Command]

Wanna freak out your friends with your mad leet command-line foo?

```for f in *.mp3; do echo -ne $f\\t; writeid3source r "${f}"; done > songlist.csv```

Edit songlist.csv then...

```while read p; do echo -ne $(echo $p| cut -f 2 -d,)\\n$(echo $p|cut -f 3 -d, )\\n | writeid3source w "$(echo $p|cut -f 1 -d,)"; done < songlist.csv ```

