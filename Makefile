# Straight forward Makefile to compile all examples in a row

INCDIR=-I${CURDIR}/include

LIBS=-lGLESv2 -lEGL -lm -lX11 -lpng

COMMONSRC=./src/Common/esShader.c    \
          ./src/Common/esTransform.c \
          ./src/Common/esShapes.c    \
          ./src/Common/esUtil.c
COMMONHRD=./include/esUtil.h

TINY_OBJ_LOADER_SRC=./src/tiny_obj_loader.cc

SRC=./src/read.cpp

all: earth.bin


draw_png_test: libcommon.a ${TINY_OBJ_LOADER_SRC} ${SRC} ./src/draw_png_test.cpp
	g++ -g -fpermissive ${INCDIR} ${LIBS}  ${TINY_OBJ_LOADER_SRC} ${SRC} ./src/draw_png_test.cpp libcommon.a -o $@ 

earth.bin: libcommon.a ${TINY_OBJ_LOADER_SRC} ${SRC} ./earth/draw_earth.cpp
	g++ -g -fpermissive ${INCDIR} ${LIBS}  ${TINY_OBJ_LOADER_SRC} ${SRC} ./earth/draw_earth.cpp libcommon.a -o earth.bin

earth: earth.bin
	./earth.bin earth/earth_universe.obj earth/earth_universe.png earth/vert.vert earth/frag.frag


libcommon.a: ${COMMONSRC} ${COMMONHRD}
	make -C src/Common
	ar -r libcommon.a src/Common/*.o

clean: dest-clean


dest-clean:
	find ./ -name '*.a' -exec rm {} -rf \;
	find ./ -name '*.o' -exec rm {} -rf \;
	find ./ -name '*.bin' -exec rm {} -rf \;
	rm -rf mytest read tiny_obj_loader.o libtinyobjloader.a draw_png_test

