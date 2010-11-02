CXX = g++
#CCFLAGS = `Magick++-config --cxxflags --cppflags --ldflags --libs`
CCFLAGS = -L. -I. -L /usr/local/lib `pkg-config --cflags --libs opencv` -lm -g


all: matscicut 

matscicut: matscicut.o matutil.o GCoptimization.o maxflow.o graph.o LinkedBlockList.o
	#${CXX} ${CCFLAGS} -o matscicut matutil.o GCoptimization.o maxflow.o graph.o LinkedBlockList.o matscicut.o
	${CXX} ${CCFLAGS} -o matscicut *.o 

matscicut.o: matscicut.cpp matscicut.h 
	${CXX} ${CCFLAGS} -c matscicut.cpp

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
