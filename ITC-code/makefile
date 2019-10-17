main: MA.o TimeTabling.o utils.o main.cpp 
	g++ -O2 -o $@ main.cpp MA.o TimeTabling.o utils.o -std=c++11

TimeTabling.o: TimeTabling.h TimeTabling.cpp
	g++ -O2 -c -o $@ TimeTabling.cpp -std=c++11

MA.o: MA.h TimeTabling.h MA.cpp
	g++ -O2 -c -o $@ MA.cpp -std=c++11

utils.o: utils.cpp utils.h
	g++ -O2 -c -o $@ utils.cpp -std=c++11

clean:
	rm -f main TimeTabling.o MA.o utils.o
