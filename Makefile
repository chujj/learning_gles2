# Straight forward Makefile to compile all examples in a row

INCDIR=-I${CURDIR}/include

LIBS=-lGLESv2 -lEGL -lm -lX11

COMMONSRC=./src/Common/esShader.c    \
          ./src/Common/esTransform.c \
          ./src/Common/esShapes.c    \
          ./src/Common/esUtil.c
COMMONHRD=./include/esUtil.h

PNG_TEST_SRC=./draw_png_test.cpp ./read.cpp

all: draw_png_test #mytest #all


draw_png_test: ${PNG_TEST_SRC} ${COMMONSRC}  libtinyobjloader.a
	g++ -g -fpermissive ${COMMONSRC} ${PNG_TEST_SRC} libtinyobjloader.a -lpng -o $@ ${INCDIR} ${LIBS}

libtinyobjloader.a: tiny_obj_loader.h tiny_obj_loader.cc
	g++ -c tiny_obj_loader.cc -o tiny_obj_loader.o
	ar -r libtinyobjloader.a tiny_obj_loader.o

libcommon.a: ${COMMONSRC} ${COMMONHRD}
	make -C src/Common
	ar -r libcommon.a src/Common/*.o

clean:
	find ./ -name '*.a' -exec rm {} -rf \;
	find ./ -name '*.o' -exec rm {} -rf \;
	rm -rf mytest read tiny_obj_loader.o libtinyobjloader.a draw_png_test

