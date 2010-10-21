CXX = g++
#CCFLAGS = `Magick++-config --cxxflags --cppflags --ldflags --libs`
CCFLAGS = -I /usr/include/opencv -L /usr/local/lib -lm `pkg-config --cflags --libs opencv`


all: gco 

gco: matscicut.o matutil.o GCoptimization.o maxflow.o graph.o LinkedBlockList.o
	${CXX} ${CCFLAGS} -o matscicut matutil.o GCoptimization.o maxflow.o graph.o LinkedBlockList.o matscicut.o

matscicut.o: matscicut.cpp matscicut.h matutil.o
	${CXX} ${CCFLAGS} -c matscicut.cpp matutil.o

matutil.o: matutil.cpp matutil.h
	${CXX} ${CCFLAGS} -c matutil.cpp
	
GCOptimization.o: GCoptimization.cpp GCoptimization.h LinkedBlockList.h energy.h graph.cpp maxflow.cpp
	${CXX} ${CCFLAGS} -c GCOptimization.cpp

maxflow.o: maxflow.cpp graph.h
	${CXX} ${CCFLAGS} -c maxflow.cpp

LinkedBlockList.o: LinkedBlockList.cpp LinkedBlockList.h
	${CXX} ${CCFLAGS} -c LinkedBlockList.cpp

graph.o: graph.h graph.cpp block.h
	${CXX} ${CCFLAGS} -c graph.cpp

clean:
	-rm *.o
	-rm matscicut 
