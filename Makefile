CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O3
WARNINGOFF = -w
PROG = ../bin/2d_3d_comparison
CTHREAD = -std=c++11 -pthread
OPENMP = -fopenmp -lgomp

SRC = main.cpp
OPENCV = `pkg-config opencv --libs --cflags`

I_ARRAY = -I../../my_libs/Array
L_ARRAY = -I../../my_libs/Array

I_CPUTIME = -I../../my_libs/CPUTime/src
L_CPUTIME = -I../../my_libs/CPUTime/bin

I_HANDLE3DDATASETS = -I../../my_libs/Handle3DDataset/src 
L_HANDLE3DDATASETS = -L../../my_libs/Handle3DDataset/bin

I_QUALITYASSESSMENT = -I../../my_libs/QualityAssessment/src 
L_QUALITYASSESSMENT = -L../../my_libs/QualityAssessment/bin

I_MI = -I../../my_libs/MI
L_MI = -L../../my_libs/MI/libMIToolbox.so

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(COPTIMIZATION) $(I_MI) $(L_MI) -o $(PROG) $(SRC) $(OPENMP) $(I_ARRAY) $(I_HANDLE3DDATASETS) $(I_QUALITYASSESSMENT) 
 
clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

