all: build

build: so-cpp.o hashmap.o linkedlist.o
	gcc so-cpp.o hashmap.o linkedlist.o -o so-cpp

so-cpp.o: so-cpp.c
hashmap.o: hashmap.c
linkedlist.o: linkedlist.c

clean:
	rm so-cpp tema1 *.o