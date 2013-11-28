CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
PROG = ../bin/2d_3d_comparison
CTHREAD = -std=c++11 -pthread
OPENMP = -fopenmp -lgomp

SRC = main.cpp 
OPENCV = `pkg-config opencv --libs --cflags`

I_CPUTIME = -I../../my_libs/CPUTime/src
L_CPUTIME = -I../../my_libs/CPUTime/bin

I_HANDLE3DDATASETS =-I../../my_libs/Handle3DDataset/src 
L_HANDLE3DDATASETS = -L../../my_libs/Handle3DDataset/bin

I_QUALITYASSESSMENT =-I../../my_libs/QualityAssessment/src 
L_QUALITYASSESSMENT = -L../../my_libs/QualityAssessment/bin


$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(COPTIMIZATION) $(OPENMP) -o $(PROG) $(SRC) $(OPENCV) $(I_HANDLE3DDATASETS) $(I_QUALITYASSESSMENT) $(I_CPUTIME) $(L_HANDLE3DDATASETS) $(L_QUALITYASSESSMENT) $(L_CPUTIME)
 
clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

