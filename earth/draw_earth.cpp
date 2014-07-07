#include "draw_earth.hpp"



#ifndef SANSHICHUAN_ANDROID_BUILD
namespace {
int Init(ESContext *esContext)
{
    UserData *userData = (UserData *)esContext->userData;
#else
int Init(UserData *userData)
{
#endif

    char * vert_shader_str;
    char * frag_shader_str;
    
    sanshichuan::readFileInStr(userData->vert_shader_file, &vert_shader_str);
    sanshichuan::readFileInStr(userData->frag_shader_file, &frag_shader_str);
    
    GLuint programObject;
    
    // create the program object
    programObject = esLoadProgram(vert_shader_str, frag_shader_str);
    free(vert_shader_str);
    free(frag_shader_str);

    if (programObject == 0) {
	return 0;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepthf(1.0f);
    
    
    // STORE the program object
    userData->programObject = programObject;

    // Get the attribute locations
    userData->positionLoc = glGetAttribLocation ( userData->programObject, "vPosition" );
    userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
    
    // Get the sampler location
    userData->textUniformLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

    printf("posLoc: %d ; texCoorLoc: %d; text: %d\n", userData->positionLoc, userData->texCoordLoc, userData->textUniformLoc);

    // load obj
    std::string inputfile = userData->objfile;
    printf("obj filename: %s\n", inputfile.c_str());
    std::vector<tinyobj::shape_t> * shapes_p = 
	new std::vector<tinyobj::shape_t>();
    std::vector<tinyobj::shape_t> &shapes = *shapes_p;

    std::string err = tinyobj::LoadObj(shapes, inputfile.c_str());
    if (!err.empty()) {
      std::cerr << err << std::endl;
      exit(1);
    }
    userData->shapes = shapes_p;

    // init speed
    std::vector<Earth_Universe_Rotate_Loc_Speed *> * speed_p =
	new std::vector<Earth_Universe_Rotate_Loc_Speed *>(shapes.size());
    std::vector<Earth_Universe_Rotate_Loc_Speed *> &speed = *speed_p;

    userData->speeds = speed_p;
    for (int i = 0; i < shapes.size(); ++i) {
    	if (shapes[i].name.find("earth") == 0) {
	    speed[i] = new Earth_Universe_Rotate_Loc_Speed(0.3, -9, 0, 1, 0);
    	} else if (shapes[i].name.find("universe") == 0) {
	    speed[i] = new Earth_Universe_Rotate_Loc_Speed (0.001, -19, 1, 1, 1);
    	} else {
    	    printf ("name not support %s\n", shapes[i].name.c_str());
    	    exit(255);
    	}
	
    }

    // load images
    int image_width, image_height;
    bool image_has_alpha;
    GLubyte *image_data;
    const char* image_filename = userData->texture_png_file;
    if (sanshichuan::loadPngImage(image_filename, image_width, image_height, image_has_alpha, &image_data)) {
	printf("load png %s success: w: %d, h: %d, alpha?: %d\n", image_filename, image_width, image_height, image_has_alpha);
    } else {
	printf("load logo.png fail\n");
	return FALSE;
    }

    GLuint tex = sanshichuan::loadTexture(image_width, image_height, image_data);
    if (tex) {
    	userData->texure = tex;
	free(image_data);
    } else {
    	printf("load tex fail\n");
	return FALSE;
    }

    // world color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    return TRUE;
}

#ifndef SANSHICHUAN_ANDROID_BUILD
void Draw(ESContext *esContext)
{
    UserData *userData = (UserData *)esContext->userData;
#else

void onSizeChange(UserData *userData, int vp_width, int vp_height)
{
    glViewport(0, 0, vp_width, vp_height);
    userData->frustumX = 1.0f * vp_width / vp_height;
}

    
    void Draw(UserData *userData)
{
#endif
    ESMatrix modelMatrix;
    ESMatrix perspectMatrix;


#ifndef SANSHICHUAN_ANDROID_BUILD
    // set the viewpoint
    glViewport(0,0, esContext->width, esContext->height);
#endif

    // clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // USe the program object
    glUseProgram(userData->programObject);

    // project matirx
    esMatrixLoadIdentity(&perspectMatrix);
    esFrustum(&perspectMatrix, -userData->frustumX, userData->frustumX, -1, 1, 3, 30);

    
    for (int i = 0; i < ((userData->shapes->size())); ++i) {
	// model matrix
	esMatrixLoadIdentity(&modelMatrix);

	esTranslate(&modelMatrix, 0, 0, userData->speeds->at(i)->translate_z);
	esRotate(&modelMatrix, userData->speeds->at(i)->angle, userData->speeds->at(i)->dx, userData->speeds->at(i)->dy, userData->speeds->at(i)->dz);
	userData->speeds->at(i)->nextframe();

	esMatrixLoadIdentity(&userData->mMVPMatrix);
	esMatrixMultiply(&userData->mMVPMatrix, &modelMatrix, &perspectMatrix);

	tinyobj::mesh_t mesh = (userData->shapes->at(i)).mesh;
	
	// load the vertex data
	std::vector<float> pos = mesh.positions;
	glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0,  &pos.front());
	glEnableVertexAttribArray(userData->positionLoc);

	// load texCoord data
	glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, & mesh.texcoords.front());
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

	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &( mesh.indices.front()));
    }

#ifndef SANSHICHUAN_ANDROID_BUILD
    eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
#endif
}

#ifndef SANSHICHUAN_ANDROID_BUILD
} // namespace
#endif


#ifndef SANSHICHUAN_ANDROID_BUILD
extern "C" {
    
int main(int argc, char *argv[])
{
    ESContext esContext;
    UserData userData;
    userData.frustumX = 1;
    
    if (argc != (4 + 1)) {
	printf("usage: ./draw_png_test objfile textpngfile vert_sharder_file frag_sharder_file\n");
	return 0;
    }

    userData.objfile = argv[1];
    userData.texture_png_file = argv[2];
    userData.vert_shader_file = argv[3];
    userData.frag_shader_file = argv[4];

    esInitContext(&esContext);
    esContext.userData = &userData;
     
    esCreateWindow(&esContext, "Earth_Universe", 400, 400, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

    if (!Init(&esContext)) {
	return 0;
    }
    esRegisterDrawFunc(&esContext, Draw);

    esMainLoop(&esContext);
    
    return 0;
}

}
#endif
