all: shell379

shell379: main.o shell379.o shell379PCB.o helpers.o
	g++ -o shell379 shell379.o shell379PCB.o main.o helpers.o

main.o: main.cpp shell379.cpp shell379PCB.hpp
	g++ -c main.cpp

shell379.o: shell379.cpp shell379.hpp shell379PCB.cpp shell379PCB.hpp helpers.o
	g++ -c shell379.cpp

helpers.o: helpers.cpp helpers.hpp
	g++ -c helpers.cpp

shell379PCB.o: shell379PCB.cpp shell379PCB.hpp
	g++ -c shell379PCB.cpp

clean:
	rm -f shell379PCB.o helpers.o shell379.o main.o shell379
