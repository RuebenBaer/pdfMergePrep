CPP = "g++.exe"
CXXFLAGS = \
	-Wall \
	-O2 \
	-std=c++14 \
	-I$(BOOSTDIR)\include\boost-1_82 \
	-I$(BOOSTDIR)\lib

LIB = \
	-L$(BOOSTDIR)\lib

BOOSTLIB = \
	-lboost_filesystem-mgw13-mt-d-x64-1_82 \
	-lboost_system-mgw13-mt-d-x64-1_82

.PHONEY: all clean

all: obj/pdfMergePrep.o
	$(CPP) $(CXXFLAGS) $^ -o pdfMergePrep.exe -s $(LIB) $(BOOSTLIB)

obj/pdfMergePrep.o: src\pdfMergePrep.cpp
	-if not exist obj mkdir obj
	$(CPP) $(CXXFLAGS) -c $^ -o $@

clean:
	-if exist bin\pdfMergePrep.exe del bin\pdfMergePrep.exe
	-if exist obj\pdfMergePrep.o del obj\pdfMergePrep.o
