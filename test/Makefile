CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CVoxelTest

SRC = \
CVoxelTest.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++14 \
-I$(INC_DIR) \
-I. \
-I../../CMath/include \
-I../../COS/include \
-I../../CUtil/include \

LFLAGS = \
-L$(LIB_DIR) \
-L../../CVoxel/lib \
-L../../CFile/lib \
-L../../CMath/lib \
-L../../COS/lib \
-L../../CStrUtil/lib \

LIBS = \
-lCVoxel \
-lCFile \
-lCMath \
-lCStrUtil \
-lCOS \

clean:
	$(RM) -f *.o
	$(RM) -f CVoxelTest

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CVoxelTest: $(OBJS) $(LIB_DIR)/libCVoxel.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CVoxelTest $(OBJS) $(LFLAGS) $(LIBS)
