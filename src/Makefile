CXX = g++
CXXFLAGS = -g -std=c++11 -Wall
LDFLAGS = -lallegro -lallegro_main \
    -lallegro_color -lallegro_font -lallegro_ttf -lallegro_acodec -lallegro_audio -lallegro_primitives -lallegro_image

all: main

main.o: main.cpp geom.hpp init.hpp ai.hpp draw.hpp physics.hpp screen.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

screen.o: screen.cpp screen.hpp geom.hpp
	$(CXX) $(CXXFLAGS) -c screen.cpp

draw.o: draw.cpp draw.hpp screen.hpp geom.hpp
	$(CXX) $(CXXFLAGS) -c draw.cpp

physics.o: physics.cpp physics.hpp geom.hpp
	$(CXX) $(CXXFLAGS) -c physics.cpp

ai.o: ai.cpp ai.hpp geom.hpp
	$(CXX) $(CXXFLAGS) -c ai.cpp

main: main.o screen.o draw.o physics.o ai.o
	$(CXX) $(CXXFLAGS) -o main $^ $(LDFLAGS)

clean:
	rm -f main *.o
