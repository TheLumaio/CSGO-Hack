
src = $(wildcard *.c) $(wildcard sha1/*.c) $(wildcard bsp/*.cc)

all:
	g++ -static -std=c++14 -fpermissive -w $(src) -o release/Pedro2.exe -lpsapi -lsapi -luuid -lole32 -loleaut32 -lpthread -lgdi32 -lpdcurses
