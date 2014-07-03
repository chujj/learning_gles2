#include "esUtil.h"
#include <string>
#include <iostream>
#include <png.h>
#include <stdlib.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "read.hpp"
#include "tiny_obj_loader.h"

namespace {

bool loadPngImage(char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData);
GLuint loadTexture(int width, int height, GLubyte *image_data);


typedef struct 
{
    // paramaters from command line
    char* vert_shader_file;
    char* frag_shader_file;
    char* objfile;
    char* texture_png_file;

    // gles related reference
    GLuint programObject;
    tinyobj::shape_t * shape;
    ESMatrix mMVPMatrix;
    GLuint texure;
    GLuint positionLoc, texCoordLoc, textUniformLoc;
} UserData;


// initialize the shader and program object
int Init(ESContext *esContext) 
{
    UserData *userData = (UserData *)esContext->userData;
    char * vert_shader_str;
    char * frag_shader_str;
    
    sanshichuan::readFileInStr(userData->vert_shader_file, &vert_shader_str);
    sanshichuan::readFileInStr(userData->frag_shader_file, &frag_shader_str);

    GLuint programObject;
    
    // create the program object
    programObject = esLoadProgram(vert_shader_str, frag_shader_str);

    if (programObject == 0) {
	return 0;
    }

    // STORE the program object
    userData->programObject = programObject;

    // Get the attribute locations
    userData->positionLoc = glGetAttribLocation ( userData->programObject, "vPosition" );
    userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
    
    // Get the sampler location
    userData->textUniformLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

    printf("posLoc: %d ; texCoorLoc: %d; text: %d\n", userData->positionLoc, userData->texCoordLoc, userData->textUniformLoc);

    // load images
    int image_width, image_height;
    bool image_has_alpha;
    GLubyte *image_data;
    char* image_filename = userData->texture_png_file;
    if (loadPngImage(image_filename, image_width, image_height, image_has_alpha, &image_data)) {
	printf("load png %s success: w: %d, h: %d, alpha?: %d\n", image_filename, image_width, image_height, image_has_alpha);
    } else {
	printf("load logo.png fail\n");
	return FALSE;
    }

    GLuint tex = loadTexture(image_width, image_height, image_data);
    if (tex) {
    	userData->texure = tex;
	free(image_data);
    } else {
    	printf("load tex fail\n");
	return FALSE;
    }

    // world color
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
GLfloat sAngleCount;
void Draw(ESContext *esContext)
{
    UserData *userData = (UserData *)esContext->userData;
    ESMatrix modelMatrix;
    ESMatrix perspectMatrix;

    // set the viewpoint
    glViewport(0,0, esContext->width, esContext->height);

    // clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // USe the program object
    glUseProgram(userData->programObject);

    if (sAngleCount ++ > 360) {
	sAngleCount = 0;
    }

//    std::cout << "degree " << sAngleCount << std::endl;

    // model matrix
    esMatrixLoadIdentity(&modelMatrix);
    esTranslate(&modelMatrix, 0.0, 0.0, -9.0);
    esRotate(&modelMatrix, sAngleCount, 1.0, 0.0, 1.0);

    // project matirx
    esMatrixLoadIdentity(&perspectMatrix);
    esFrustum(&perspectMatrix, -1, 1, -1, 1, 3, 30);
//    float aspect = (GLfloat) esContext->width / (GLfloat) esContext->height;
//    esPerspective(&perspectMatrix, 60.0f, aspect, -0, -0.3);

    esMatrixMultiply(&userData->mMVPMatrix, &modelMatrix, &perspectMatrix);
    
    // load the vertex data
    tinyobj::mesh_t mesh = userData->shape->mesh;
    std::vector<float> pos = mesh.positions;
    glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, pos.data());
    glEnableVertexAttribArray(userData->positionLoc);

    // load texCoord data
    glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, mesh.texcoords.data());
    glEnableVertexAttribArray(userData->texCoordLoc);

    // load the MVP matrix
    glUniformMatrix4fv(
	glGetUniformLocation(userData->programObject, "uModelMatrix"), 1, false, (GLfloat*)&userData->mMVPMatrix);

    /// load the texture
    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, userData->texure );
    
    // Set the sampler texture unit to 0
    glUniform1i ( userData->textUniformLoc, 0 );

    
//    glDrawArrays(GL_TRIANGLES, 0, gVertices.size());
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, mesh.indices.data());
    
    eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
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

bool loadPngImage(char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData) {
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


}




extern "C" {
    
int main(int argc, char *argv[])
{
    ESContext esContext;
    UserData userData;

    if (argc != (4 + 1)) {
	printf("usage: ./draw_png_test objfile textpngfile vert_sharder_file frag_sharder_file\n");
	return 0;
    }

    userData.objfile = argv[1];
    userData.texture_png_file = argv[2];
    userData.vert_shader_file = argv[3];
    userData.frag_shader_file = argv[4];

    std::string inputfile = userData.objfile;
    std::vector<tinyobj::shape_t> shapes;
  
    std::string err = tinyobj::LoadObj(shapes, inputfile.c_str());
  
    if (!err.empty()) {
      std::cerr << err << std::endl;
      exit(1);
    }

    userData.shape = &shapes[0];

    esInitContext(&esContext);
    esContext.userData = &userData;
     
    esCreateWindow(&esContext, "ZHUJJ First GLES2 demo", 400, 400, ES_WINDOW_RGB);

    if (!Init(&esContext)) {
	return 0;
    }
    esRegisterDrawFunc(&esContext, Draw);

    esMainLoop(&esContext);
    
    return 0;
}

}
