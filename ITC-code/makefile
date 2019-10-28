main: MA.o TimeTabling.o Individual.o utils.o pugixml.o main.cpp 
	g++ -O2 -o $@ main.cpp MA.o TimeTabling.o Individual.o utils.o pugixml.o -std=c++11

TimeTabling.o: TimeTabling.h TimeTabling.cpp
	g++ -O2 -c -o $@ TimeTabling.cpp -std=c++11

Individual.o: Individual.h Individual.cpp
	g++ -O2 -c -o $@ Individual.cpp -std=c++11

MA.o: MA.h TimeTabling.h MA.cpp
	g++ -O2 -c -o $@ MA.cpp -std=c++11

utils.o: utils.cpp utils.h
	g++ -O2 -c -o $@ utils.cpp -std=c++11

pugixml.o: pugixml.hpp pugixml.cpp
	g++ -O2 -c -o $@ pugixml.cpp -std=c++11

clean:
	#rm -f main TimeTabling.o MA.o utils.o
	rm -f main TimeTabling.o Individual.o MA.o utils.o pugixml.o 
