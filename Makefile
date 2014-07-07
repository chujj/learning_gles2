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

earth.bin: libcommon.a ${TINY_OBJ_LOADER_SRC} ${SRC} ./earth/draw_earth.cpp ./earth/draw_earth.hpp
	g++ -g -fpermissive ${INCDIR} ${LIBS}  ${TINY_OBJ_LOADER_SRC} ${SRC} ./earth/draw_earth.cpp libcommon.a -o earth.bin

earth.android: ${COMMONSRC} ${COMMONHRD} ${TINY_OBJ_LOADER_SRC} ${SRC} ./earth/draw_earth.cpp ./earth/draw_earth.hpp
	cp ${COMMONSRC} ${COMMONHRD} ${TINY_OBJ_LOADER_SRC} ${SRC} android/jni/src
	cp include/* android/jni/src
	cp ./earth/draw_earth.cpp ./earth/draw_earth.hpp android/jni/src
	cp ./earth/earth_universe.obj ./earth/earth_universe.png ./earth/vert.vert ./earth/frag.frag android/assets
	(cd android ; ndk-build)

earth.android.clean:
	(cd android; ndk-build clean)

earth: earth.bin
	./earth.bin earth/earth_universe.obj earth/earth_universe.png earth/vert.vert earth/frag.frag

dump_matrix.bin: src/dump_matrix.cpp libcommon.a ${SRC}
	g++ -g -fpermissive ${INCDIR} ${LIBS} ${SRC} src/dump_matrix.cpp libcommon.a -o dump_matrix.bin &&  ./dump_matrix.bin


libcommon.a: ${COMMONSRC} ${COMMONHRD}
	make -C src/Common
	ar -r libcommon.a src/Common/*.o

clean: dest-clean


dest-clean: earth.android.clean
	find ./ -name '*.a' -exec rm {} -rf \;
	find ./ -name '*.o' -exec rm {} -rf \;
	find ./ -name '*.bin' -exec rm {} -rf \;
	rm -rf mytest read tiny_obj_loader.o libtinyobjloader.a draw_png_test

