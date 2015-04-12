EXECUTABLE = HyperNeat
VPATH = ./src ./headers ./objects
CC = g++ -O3
CFLAGS = -g -Wall -fPIC -I./headers 
LDFLAGS = -lpthread -L. -lneat
OBJS = ./objects/HyperNeat.o ./objects/Substrate.o ./objects/SpatialNode.o ./objects/CPPNInputs.o 

all: HyperNeat.o Substrate.o SpatialNode.o CPPNInputs.o   	
	
HyperNeat.o: HyperNeat.cpp
	@mkdir -p objects
	@echo "Compiling HyperNeat.cpp to HyperNeat.o"
	@$(CC) $(CFLAGS) -c ./src/HyperNeat.cpp  -o ./objects/HyperNeat.o

Substrate.o: Substrate.cpp
	@mkdir -p objects
	@echo "Compiling Substrate.cpp to Substrate.o"
	@$(CC) $(CFLAGS) -c ./src/Substrate.cpp  -o ./objects/Substrate.o

SpatialNode.o: SpatialNode.cpp
	@mkdir -p objects
	@echo "Compiling SpatialNode.cpp to SpatialNode.o"
	@$(CC) $(CFLAGS) -c ./src/SpatialNode.cpp  -o ./objects/SpatialNode.o

CPPNInputs.o: CPPNInputs.cpp 
	@mkdir -p objects
	@echo "Compiling CPPNInputs.cpp to CPPNInputs.o"
	@$(CC) $(CFLAGS) -c ./src/CPPNInputs.cpp  -o ./objects/CPPNInputs.o

clean:
	@rm -f -R ./objects

cleandocs:
	@rm -f -R ./doc

install:
	g++ -shared -Wl,-soname,libhyperneat.so.1 -o libhyperneat.so.1.0 $(OBJS) $(LDFLAGS)
	ln -sf libhyperneat.so.1.0 libhyperneat.so
	ln -sf libhyperneat.so.1.0 libhyperneat.so.1
	mv libhyperneat.so.1.0 libhyperneat.so libhyperneat.so.1 /usr/lib
	mkdir -p /usr/include/HYPERNEAT_headers/
	cp ./headers/* /usr/include/HYPERNEAT_headers/
	cp HYPERNEAT /usr/include
	chmod go+r /usr/include/HYPERNEAT_headers/*
	chmod go+r /usr/include/HYPERNEAT

docs:
	@mkdir -p doc
	@doxygen HYPERNEAT_doxyfile 

git:
	make clean
	make cleandocs
	git add --all
	git commit -m "$(commit)"
	git push