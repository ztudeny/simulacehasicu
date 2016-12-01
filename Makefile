CC=g++
SOURCE=hasici.cc hasici.h
EXECUTABLE=hasici
LFLAGS= -lsimlib -lm 
CFLAGS = -std=c++11 -g -O2

all: 
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS) -o $(EXECUTABLE)

pack:
	tar -cvf xpawlu00.tar Makefile hasici.cc hasici.h
clean:
	rm -f hasici
run:
	./$(EXECUTABLE)
