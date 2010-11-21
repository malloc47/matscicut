OBJ = obj
INC = include
SRC = src
GCOPATH = ~/src/programs/gco/
BLOBPATH = ~/src/programs/cvblobslib/src/
CXX = g++
#CCFLAGS = `Magick++-config --cxxflags --cppflags --ldflags --libs`
CCFLAGS = -L. -I. -I${INC} -I${SRC} -L /usr/local/lib `pkg-config --cflags --libs opencv` -lm

all: matscicut 

matscicut: ${OBJ}/matscicut.o ${OBJ}/matutil.o ${OBJ}/GCoptimization.o ${OBJ}/maxflow.o ${OBJ}/graph.o ${OBJ}/LinkedBlockList.o
	#${CXX} ${CCFLAGS} -o matscicut ${OBJ}/matutil.o ${OBJ}/GCoptimization.o ${OBJ}/maxflow.o ${OBJ}/graph.o ${OBJ}/LinkedBlockList.o ${OBJ}/matscicut.o
	${CXX} ${CCFLAGS} -o matscicut ${OBJ}/*.o 

${OBJ}/matscicut.o: ${SRC}/matscicut.cpp ${INC}/matscicut.h 
	${CXX} ${CCFLAGS} -c ${SRC}/matscicut.cpp -o ${OBJ}/matscicut.o

${OBJ}/matutil.o: ${SRC}/matutil.cpp ${INC}/matutil.h
	${CXX} ${CCFLAGS} -c ${SRC}/matutil.cpp -o ${OBJ}/matutil.o
	
${OBJ}/GCoptimization.o: ${SRC}/GCoptimization.cpp ${INC}/GCoptimization.h ${INC}/LinkedBlockList.h ${INC}/energy.h ${SRC}/graph.cpp ${SRC}/maxflow.cpp
	${CXX} ${CCFLAGS} -c ${SRC}/GCoptimization.cpp -o ${OBJ}/GCoptimization.o

${OBJ}/maxflow.o: ${SRC}/maxflow.cpp ${OBJ}/graph.o 
	${CXX} ${CCFLAGS} -c ${SRC}/maxflow.cpp -o ${OBJ}/maxflow.o

${OBJ}/LinkedBlockList.o: ${SRC}/LinkedBlockList.cpp ${INC}/LinkedBlockList.h
	${CXX} ${CCFLAGS} -c ${SRC}/LinkedBlockList.cpp -o ${OBJ}/LinkedBlockList.o

${OBJ}/graph.o: ${INC}/graph.h ${SRC}/graph.cpp ${INC}/block.h
	${CXX} ${CCFLAGS} -c ${SRC}/graph.cpp -o ${OBJ}/graph.o

init:
	mkdir obj
	-ln -s ${BLOBPATH}/*.o ${OBJ}/
	-ln -s ${BLOBPATH}/*.h ${INC}/
	-ln -s ${GCOPATH}/*.cpp  ${SRC}/
	-ln -s ${GCOPATH}/*.h ${INC}/
clean:
	-rm ${OBJ}/*.o
	-rm matscicut 
#	for f in `ls ${BLOBPATH}/*.o` do ; rm 
