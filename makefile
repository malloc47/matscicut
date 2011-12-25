OBJ = obj
INC = include
SRC = src
GCOPATH = /home/malloc47/src/programs/gco
BLOBPATH = /home/malloc47/src/programs/cvblobslib
CXX = g++
CCFLAGS = `pkg-config --cflags opencv` -I. -I${INC} -I${SRC} -I${GCOPATH} -I${BLOBPATH}/include 
LDFLAGS = `pkg-config --libs opencv` -L. -L/usr/local/lib -L${GCOPATH} -L${BLOBPATH}/src -lblob -lgco -lm

all: matscicut 

matscicut: ${OBJ}/matscicut.o ${OBJ}/matutil.o
	${CXX} ${OBJ}/*.o ${CCFLAGS} ${LDFLAGS} -o matscicut

${OBJ}/matscicut.o: ${SRC}/matscicut.cpp ${INC}/matscicut.h 
	-mkdir obj
	${CXX} ${CCFLAGS} -c ${SRC}/matscicut.cpp -o ${OBJ}/matscicut.o

${OBJ}/matutil.o: ${SRC}/matutil.cpp ${INC}/matutil.h
	${CXX} ${CCFLAGS} -c ${SRC}/matutil.cpp -o ${OBJ}/matutil.o

clean:
	-rm ${OBJ}/*.o
	-rm matscicut 
