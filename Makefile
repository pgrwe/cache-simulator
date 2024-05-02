CC=g++
SRC=./src/
BIN=./bin/
OBJ=./obj/
all : main.o
	$(CC) $(OBJ)main.o -o $(BIN)main

main.o: $(SRC)main.cpp
	$(CC) -c $(SRC)main.cpp -o $(OBJ)main.o

clean:
	rm -rf $(BIN)* $(OBJ)*.o
