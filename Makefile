all: pat pat2 pat3 howtouse sh nagoya qr2txt pow

pow: pow.cc
	$(CXX) -o $@ $< -g -O -lssl -lcrypto

pat: pat.cc
	$(CXX) -o $@ $< -g -O

qr2txt: qr2txt.cc
	$(CXX) -o $@ $< -g -O -lSDL `sdl-config --cflags`

pat2: pat2.cc
	$(CXX) -o $@ $< -g -O -std=c++11

pat3: pat3.cc
	$(CXX) -o $@ $< -g -O -std=c++11

nagoya: nagoya.cc
	$(CXX) -o $@ $< -g -O -std=c++11 -lSDL `sdl-config --cflags`

sh: sh.c
	$(CC) -o $@ $< -g -O -m32 -ldl -Wl,-Ttext-segment=0x2000000

howtouse: howtouse.cc
	$(CXX) -o $@ $< -g -O -m32
