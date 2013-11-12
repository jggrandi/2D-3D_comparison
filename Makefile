CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
PROG = ../bin/2d_3d_comparison
CTHREAD = -std=c++11 -pthread

SRC = main.cpp 
OPENCV = `pkg-config opencv --libs --cflags`

MY_INCLUDE_CPUTIME = -I../../my_libs/CPUTime/src
MY_LIB_PATCH_CPUTIME = -I../../my_libs/CPUTime/bin

MY_INCLUDE_HANDLE3DDATASETS =-I../../my_libs/Handle3DDataset/src 
MY_LIB_PATCH_HANDLE3DDATASETS = -L../../my_libs/Handle3DDataset/bin

MY_INCLUDE_QUALITYASSESSMENT =-I../../my_libs/QualityAssessment/src 
MY_LIB_QUALITYASSESSMENT = -lqualityassessment
MY_LIB_PATCH_QUALITYASSESSMENT = -L../../my_libs/QualityAssessment/bin
MY_R_PATCH_QUALITYASSESSMENT = -Wl,-R../../my_libs/QualityAssessment/bin


$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) $(CTHREAD) -o $(PROG) $(SRC) $(OPENCV) $(MY_INCLUDE_HANDLE3DDATASETS) $(MY_INCLUDE_QUALITYASSESSMENT) $(MY_LIB_HANDLE3DDATASETS) $(MY_LIB_QUALITYASSESSMENT) $(MY_LIB_PATCH_HANDLE3DDATASETS) $(MY_LIB_PATCH_QUALITYASSESSMENT) $(MY_R_PATCH_HANDLE3DDATASETS) $(MY_R_PATCH_QUALITYASSESSMENT) $(MY_INCLUDE_CPUTIME) $(MY_LIB_PATCH_CPUTIME)
 
clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

