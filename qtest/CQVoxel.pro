TEMPLATE = app

TARGET = CQVoxel

DEPENDPATH += .

QT += widgets opengl

QMAKE_CXXFLAGS += -std=c++14

SOURCES += \
CQVoxel.cpp \
CQGLControl.cpp \
CGLTexture.cpp \
CGLUtil.cpp \

HEADERS += \
CQVoxel.h \
CQGLControl.h \
CGLTexture.h \
CGLUtil.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../../CQUtil/include \
../../CVoxel/include \
../../CImageLib/include \
../../CFont/include \
../../CFile/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \
.

unix:LIBS += \
-L$$LIB_DIR \
-L../../CQUtil/lib \
-L../../CConfig/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CFile/lib \
-L../../CFileUtil/lib \
-L../../CMath/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../CUtil/lib \
-L../../COS/lib \
-lCVoxel \
-lCQUtil \
-lCConfig \
-lCImageLib \
-lCFont \
-lCFile \
-lCFileUtil \
-lCMath \
-lCStrUtil \
-lCRegExp \
-lCUtil \
-lCOS \
-lglut -lGLU -lGL -lpng -ljpeg -ltre
