#all:
#	g++ -g --std=c++11 -DNDEBUG -I ./minisat/include -I ./minisat/solver -O2 main.cc Options.cc SpecificationFile.cc PascalTriangle.cc Combinadic.cc  LocalSearch.cc Coverage.cc CoveringArray.cc TupleSet.cc mersenne.cc -o main_paws
#
#src = ${shell ls *.cc}
#objs = ${patsubst %.cc, %.o, $(src)}

objs = Options.o SpecificationFile.o PascalTriangle.o Combinadic.o  ConstraintFile.o\
	   LocalSearch.o LineVarTupleSet.o Coverage.o CoveringArray.o TupleSet.o mersenne.o SAT.o Sovler.o main.o

CC = g++
CFLAGS = --std=c++0x -DNDEBUG -O2

all : TCA

TCA : $(objs)
	$(CC) $(CFLAGS) -o $@ $^

main.o : main.cc
	$(CC) $(CFLAGS) -c -o $@ $^

Sovler.o : ./minisat/solver/Solver.C
	$(CC) $(CFLAGS)  -I ./minisat/include -c -o $@ $^

SAT.o : SAT.C
	$(CC) $(CFLAGS) -c -o $@ $^

LineVarTupleSet.o : LineVarTupleSet.cc
	$(CC) $(CFLAGS) -c -o $@ $^

ConstraintFile.o : ConstraintFile.C
	$(CC) $(CFLAGS) -c -o $@ $^

Options.o : Options.cc
	$(CC) $(CFLAGS) -c -o $@ $^

SpecificationFile.o : SpecificationFile.cc
	$(CC) $(CFLAGS) -c -o $@ $^

PascalTriangle.o : PascalTriangle.cc
	$(CC) $(CFLAGS) -c -o $@ $^

Combinadic.o : Combinadic.cc
	$(CC) $(CFLAGS) -c -o $@ $^

LocalSearch.o : LocalSearch.cc
	$(CC) $(CFLAGS) -c -o $@ $^

Coverage.o : Coverage.cc
	$(CC) $(CFLAGS) -c -o $@ $^

CoveringArray.o : CoveringArray.cc
	$(CC) $(CFLAGS) -c -o $@ $^

TupleSet.o : TupleSet.cc
	$(CC) $(CFLAGS) -c -o $@ $^

mersenne.o : mersenne.cc
	$(CC) $(CFLAGS) -c -o $@ $^

#%.o : %.cc
#	$(CC) $(CFLAGS) -o $@ $^

clean :
	- rm *.o
	- rm TCA
