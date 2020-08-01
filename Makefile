

all : 
	cd addtimestamp; make; cd ..
	cd discordposter; make; cd ..
	cd streambuddy; make; cd ..
	cd ytchatmon; make; cd ..
	cd ytoauthhelper; make; cd ..
	cd ytposter; make; cd ..
	cd ytstreamstats; make; cd ..
	cd id3tool; make install; cd ..



initial :
	git submodule update --init --recursive
	git pull --recurse-submodules
	( cd cntools/tccengine/tcc && make libtcc_x86_64.a libtcc_i686.a include_tcc  ) || die

