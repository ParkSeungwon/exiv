CFLAG = -std=c++11 -g -fmax-errors=1 
CC = g++
SRC = $(wildcard *.cc)
CPP = $(wildcard *.cpp)
EXE = $(patsubst %.cpp, %.x, $(CPP))
OBJ = $(patsubst %.cc, %.o, $(SRC))

all : $(OBJ) $(EXE) 

%.x : %.cpp $(OBJ)
	$(CC) $< -o $@ $(OBJ) $(CFLAG) -lexiv2

%.o : %.cc 
	$(CC) $< -c $(CFLAG)

clean :
	rm *.o *.x
