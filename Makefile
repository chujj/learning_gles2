# Straight forward Makefile to compile all examples in a row

INCDIR=-I${CURDIR}/include

LIBS=-lGLESv2 -lEGL -lm -lX11 -lpng

COMMONSRC=./src/Common/esShader.c    \
          ./src/Common/esTransform.c \
          ./src/Common/esShapes.c    \
          ./src/Common/esUtil.c
COMMONHRD=./include/esUtil.h

TINY_OBJ_LOADER_SRC=./src/tiny_obj_loader.cc
TINY_OBJ_LOADER_HEADER=./src/tiny_obj_loader.h

PNG_TEST_SRC=./src/draw_png_test.cpp ./src/read.cpp

all: draw_png_test #mytest #all


draw_png_test: libcommon.a ${TINY_OBJ_LOADER_HEADER} ${TINY_OBJ_LOADER_SRC} ${PNG_TEST_SRC}
	g++ -g -fpermissive ${INCDIR} ${LIBS}  ${TINY_OBJ_LOADER_SRC} ${PNG_TEST_SRC} libcommon.a -o $@ 

libcommon.a: ${COMMONSRC} ${COMMONHRD}
	make -C src/Common
	ar -r libcommon.a src/Common/*.o

clean: dest-clean


dest-clean:
	find ./ -name '*.a' -exec rm {} -rf \;
	find ./ -name '*.o' -exec rm {} -rf \;
	rm -rf mytest read tiny_obj_loader.o libtinyobjloader.a draw_png_test

