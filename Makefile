CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
PROG = ../bin/2d_3d_comparison

SRC = main.cpp 

MY_INCLUDE_HANDLE3DDATASETS =-I/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/Handle3DDataset/src 
MY_LIB_HANDLE3DDATASETS = -lhandle3ddataset
MY_LIB_PATCH_HANDLE3DDATASETS = -L/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/Handle3DDataset/bin
MY_R_PATCH_HANDLE3DDATASETS = -Wl,-R/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/Handle3DDataset/bin

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) -o $(PROG) $(SRC) $(MY_INCLUDE_HANDLE3DDATASETS) $(MY_LIB_HANDLE3DDATASETS)  $(MY_LIB_PATCH_HANDLE3DDATASETS) $(MY_R_PATCH_HANDLE3DDATASETS)


clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)
