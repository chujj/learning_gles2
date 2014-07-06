#include "read.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

using std::cout;
using std::endl;
using std::cin;

namespace sanshichuan {


// real use absFilename's (size + 1) into data buff
long readFileInStr(const char * absFilename, char ** data)
{
    FILE * f = fopen(absFilename, "r");
    if (f == NULL) {
	cout << "open file fail of" << absFilename << endl;
	return -1l;
    }

    // get the size
    long size = 0;
    fseek(f, 0l, SEEK_END);
    size = ftell(f);
    // cout << "size of file: " << size <<endl;
    fseek(f, 0l, SEEK_SET); // reset to head

    char * buffer = (char *) malloc(sizeof(char) * (size + 1));
    memset(buffer, 0, sizeof(char) * (1 + size));
    long retval = fread(buffer, sizeof(char), size, f);
    if (retval != size) {
	cout << "read fail ";
	cout << "read " << retval << " total: " << size <<endl;
    } else {
	// cout << "read success" << endl;
    }

    *data = buffer;
    return size;
}


void dumpMatrix(ESMatrix & matrix) 
{
    for (int i = 0; i < 4; ++i) {
	for (int j = 0; j < 4; ++j) {
	    printf("%f ", matrix.m[j][i]);
	}
	printf("\n");
    }
    printf("\n");
}

void dumpShapes(std::vector<tinyobj::shape_t> &shapes)
{
    for (int i = 0; i < shapes.size(); ++i) {
	tinyobj::shape_t shape = shapes[i];
	printf("shape %d name: %s\n", i ,shape.name.c_str());

	std::vector<unsigned int> indexs = shape.mesh.indices;
	printf("indices: ");
	for (int i = 0; i < indexs.size(); ++i) {
	    printf(" %d ", indexs[i]);
	}
	printf("\n");

	std::vector<float> position = shape.mesh.positions;
	printf("positions: \n");
	for (int i = 0; i < position.size(); ++i) {
	    printf(" %f ", position[i]);
	    if ((i + 1) % 3 == 0)
		printf("\n");
	}

	printf("\n");
    }


}

bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData) {
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;
    FILE *fp;

    if ((fp = fopen(name, "rb")) == NULL)
        return false;

    /* Create and initialize the png_struct
     * with the desired error handler
     * functions.  If you want to use the
     * default stderr and longjump method,
     * you can supply NULL for the last
     * three parameters.  We also supply the
     * the compiler header file version, so
     * that we know if the application
     * was compiled with a compatible version
     * of the library.  REQUIRED
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
            NULL, NULL, NULL);

    if (png_ptr == NULL) {
        fclose(fp);
        return false;
    }

    /* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
        return false;
    }

    /* Set error handling if you are
     * using the setjmp/longjmp method
     * (this is the normal method of
     * doing things with libpng).
     * REQUIRED unless you  set up
     * your own error handlers in
     * the png_create_read_struct()
     * earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        /* If we get here, we had a
         * problem reading the file */
        return false;
    }

    /* Set up the output control if
     * you are using standard C streams */
    png_init_io(png_ptr, fp);

    /* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);

    /*
     * If you have enough memory to read
     * in the entire image at once, and
     * you need to specify only
     * transforms that can be controlled
     * with one of the PNG_TRANSFORM_*
     * bits (this presently excludes
     * dithering, filling, setting
     * background, and doing gamma
     * adjustment), then you can read the
     * entire image (including pixels)
     * into the info structure with this
     * call
     *
     * PNG_TRANSFORM_STRIP_16 |
     * PNG_TRANSFORM_PACKING  forces 8 bit
     * PNG_TRANSFORM_EXPAND forces to
     *  expand a palette into RGB
     */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, png_voidp_NULL);

    outWidth = info_ptr->width;
    outHeight = info_ptr->height;
    printf("width %d ; height %d \n", outWidth, outHeight);
    switch (info_ptr->color_type) {
        case PNG_COLOR_TYPE_RGBA:
            outHasAlpha = true;
            break;
        case PNG_COLOR_TYPE_RGB:
            outHasAlpha = false;
            break;
        default:
            std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
            return false;
    }
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    *outData = (unsigned char*) malloc(row_bytes * outHeight);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    for (int i = 0; i < outHeight; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(*outData+(row_bytes * (outHeight-1-i)), row_pointers[i], row_bytes);
    }

    /* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    /* Close the file */
    fclose(fp);

    /* That's it */
    return true;
}

GLuint loadTexture(int width, int height, GLubyte *image_data)
{
    GLuint tex = 0;

    // use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // generate texture object
    glGenTextures(1, &tex);
    printf("text generate: %d\n" , tex);

    // bind tex_object to operate on it
    glBindTexture(GL_TEXTURE_2D, tex);

    // upload the images
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);

    // set texture filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return tex;
}

// int main(int argc, char *argv[])
// {
//     if (argc < 2) {
// 	cout<<" use me like: ./read file need to read" << endl;
// 	exit(255);
//     }

//     char * pdata;
//     printf("addr: %x\n" , pdata);
    
//     long readsize = readFileInStr(argv[1], &pdata);
//     printf("addr: %x\n" , pdata);
//     cout << readsize << endl;
//     cout << pdata;
//     cout << std::ends;
    
//     return 0;
// }



}
