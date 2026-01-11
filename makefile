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

all: pdfMergePrep.exe bildDoku.exe

pdfMergePrep.exe: obj/pdfMergePrep.o
	$(CPP) $(CXXFLAGS) $^ -o pdfMergePrep.exe -s $(LIB) $(BOOSTLIB)

bildDoku.exe: obj/bildDokuLaTeX.o
	$(CPP) $(CXXFLAGS) $^ -o bildDoku.exe -s $(LIB) $(BOOSTLIB)
	

obj/pdfMergePrep.o: src/pdfMergePrep.cpp
	-if not exist obj mkdir obj
	$(CPP) $(CXXFLAGS) -c $^ -o $@

obj/bildDokuLaTeX.o: src/bildDokuLaTeX.cpp
	-if not exist obj mkdir obj
	$(CPP) $(CXXFLAGS) -c $^ -o $@


clean:
	-if exist pdfMergePrep.exe del pdfMergePrep.exe
	-if exist bildDoku.exe del bildDoku.exe
	-if exist obj\pdfMergePrep.o del obj\pdfMergePrep.o
