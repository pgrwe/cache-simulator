CC=g++
SRC=./src/
OBJ=./obj/
TRACES=./traces/

run : all
	./cache-sim $(TRACES)trace2.txt output.txt

all : main.o
	$(CC) $(OBJ)main.o -o cache-sim

main.o: $(SRC)main.cpp
	$(CC) -c $(SRC)main.cpp -o $(OBJ)main.o

clean:
	rm -rf $(OBJ)*.o


