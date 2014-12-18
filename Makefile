CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O3
WARNINGOFF = -w
PROG = ../bin/2d_3d_comparison
CTHREAD = -std=c++11 -pthread
OPENMP = -fopenmp -lgomp

SRC = main.cpp


I_ARRAY = -I../../my_libs/Array
L_ARRAY = -L../../my_libs/Array

I_CPUTIME = -I../../my_libs/CPUTime/src
L_CPUTIME = -I../../my_libs/CPUTime/bin

I_HANDLE3DDATASETS = -I../../my_libs/Handle3DDataset/src 
L_HANDLE3DDATASETS = -L../../my_libs/Handle3DDataset/bin

I_QUALITYASSESSMENT = -I../../my_libs/QualityAssessment/src 
L_QUALITYASSESSMENT = -L../../my_libs/QualityAssessment/bin

ICGAL = -I/usr/include/CGAL
LCGAL = -lCGAL

I_IQA = -I../../iqa_1.1.2/include
L_IQA = -L../../iqa_1.1.2/build/release 

I_MI  = -I../../MI
L_MI  = -L../../MI -Wl,-rpath=../../MI

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(COPTIMIZATION) --std=c++11   $(I_ARRAY) $(I_HANDLE3DDATASETS) $(I_QUALITYASSESSMENT)  $(I_IQA) $(L_IQA) $(ICGAL) $(LCGAL) -o $(PROG) $(SRC)  $(OPENMP) -liqa 
 
clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

