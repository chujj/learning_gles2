#include "draw_earth.hpp"

const static float kFrustumNormal = 500;

#ifndef SANSHICHUAN_ANDROID_BUILD
namespace {
void drawearth(UserData* userData, int earth_idx, const ESMatrix * perspectMatrix) ;
void drawuniverse(UserData* userData, int universe_idx, const ESMatrix * perspectMatrix);

int Init(ESContext *esContext)
{
    UserData *userData = (UserData *)esContext->userData;
#else
void drawearth(UserData* userData, int earth_idx, const ESMatrix * perspectMatrix) ;
void drawuniverse(UserData* userData, int universe_idx, const ESMatrix * perspectMatrix);

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
    
    userData->drawearth = drawearth;
    userData->drawuniverse = drawuniverse;
    
    // STORE the program object
    userData->programObject = programObject;

    // Get the attribute locations
    userData->positionLoc = glGetAttribLocation ( userData->programObject, "vPosition" );
    userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
    userData->normalLoc =   glGetAttribLocation(  userData->programObject, "a_normal" );
    
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
	    speed[i]->use_light = 1.0;
	    userData->earth_idx = i;
    	} else if (shapes[i].name.find("universe") == 0) {
	    speed[i] = new Earth_Universe_Rotate_Loc_Speed (0.001, -9, 1, 1, 1);
	    userData->universe_idx = i;
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


void drawearth(UserData* userData, int earth_idx, const ESMatrix * perspectMatrix) 
{
    ESMatrix modelMatrix;
    
    // model matrix
    esMatrixLoadIdentity(&modelMatrix);

    esTranslate(&modelMatrix, 0, 0, userData->speeds->at(earth_idx)->translate_z);
    esRotate(&modelMatrix,
	     userData->speeds->at(earth_idx)->angle,
	     userData->speeds->at(earth_idx)->dx,
	     userData->speeds->at(earth_idx)->dy,
	     userData->speeds->at(earth_idx)->dz);
    userData->speeds->at(earth_idx)->nextframe();

    tinyobj::mesh_t mesh = (userData->shapes->at(earth_idx)).mesh;
	
    // load the vertex data
    std::vector<float> pos = mesh.positions;
    glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0,  &pos.front());
    glEnableVertexAttribArray(userData->positionLoc);

    // load the normal data
    std::vector<float> normals= mesh.normals;
    glVertexAttribPointer(userData->normalLoc, 3, GL_FLOAT,   GL_FALSE, 0,  &normals.front());
    glEnableVertexAttribArray(userData->normalLoc);
	
    // load texCoord data
    glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, & mesh.texcoords.front());
    glEnableVertexAttribArray(userData->texCoordLoc);

    // load the MVP matrix
    glUniformMatrix4fv(
	glGetUniformLocation(userData->programObject, "uModelMatrix"), 1, false, (GLfloat*)&modelMatrix);
    glUniformMatrix4fv(
	glGetUniformLocation(userData->programObject, "uProjectionMatrix"), 1, false, (GLfloat*)perspectMatrix);
    glUniform1f(
	glGetUniformLocation(userData->programObject, "u_useLight"),
	userData->speeds->at(earth_idx)->use_light);
	
    /// load the texture
    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, userData->texure );
	
    // Set the sampler texture unit to 0
    glUniform1i ( userData->textUniformLoc, 0 );

    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &( mesh.indices.front()));
}

void drawuniverse(UserData* userData, int universe_idx, const ESMatrix * perspectMatrix)
{
    ESMatrix modelMatrix;
    
    // model matrix
    esMatrixLoadIdentity(&modelMatrix);

    esTranslate(&modelMatrix, 0, 0, userData->speeds->at(universe_idx)->translate_z);
    esRotate(&modelMatrix,
	     userData->speeds->at(universe_idx)->angle,
	     userData->speeds->at(universe_idx)->dx,
	     userData->speeds->at(universe_idx)->dy,
	     userData->speeds->at(universe_idx)->dz);
    userData->speeds->at(universe_idx)->nextframe();

    tinyobj::mesh_t mesh = (userData->shapes->at(universe_idx)).mesh;
	
    // load the vertex data
    std::vector<float> pos = mesh.positions;
    glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0,  &pos.front());
    glEnableVertexAttribArray(userData->positionLoc);

    // load the normal data
    std::vector<float> normals= mesh.normals;
    glVertexAttribPointer(userData->normalLoc, 3, GL_FLOAT,   GL_FALSE, 0,  &normals.front());
    glEnableVertexAttribArray(userData->normalLoc);
	
    // load texCoord data
    glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, & mesh.texcoords.front());
    glEnableVertexAttribArray(userData->texCoordLoc);

    // load the MVP matrix
    glUniformMatrix4fv(
	glGetUniformLocation(userData->programObject, "uModelMatrix"), 1, false, (GLfloat*)&modelMatrix);
    glUniformMatrix4fv(
	glGetUniformLocation(userData->programObject, "uProjectionMatrix"), 1, false, (GLfloat*)perspectMatrix);
    glUniform1f(
	glGetUniformLocation(userData->programObject, "u_useLight"),
	userData->speeds->at(universe_idx)->use_light);
	
    /// load the texture
    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, userData->texure );
	
    // Set the sampler texture unit to 0
    glUniform1i ( userData->textUniformLoc, 0 );

    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &( mesh.indices.front()));
}


#ifndef SANSHICHUAN_ANDROID_BUILD
void onSizeChange(ESContext *esContext)
{
    ((UserData*) esContext->userData)->frustumX = 1.0f * esContext->width / kFrustumNormal;
    ((UserData*) esContext->userData)->frustumY = 1.0f * esContext->height / kFrustumNormal;
    glViewport(0, 0, esContext->width, esContext->height);
}

void Draw(ESContext *esContext)
{
    UserData *userData = (UserData *)esContext->userData;
#else

void onSizeChange(UserData *userData, int vp_width, int vp_height)
{
    glViewport(0, 0, vp_width, vp_height);
    userData->frustumX = 1.0f * vp_width / kFrustumNormal;
    userData->frustumY = 1.0f * vp_height / kFrustumNormal;
}

    
    void Draw(UserData *userData)
{
#endif
    ESMatrix modelMatrix;
    ESMatrix perspectMatrix;

    // clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // USe the program object
    glUseProgram(userData->programObject);

    // project matirx
    esMatrixLoadIdentity(&perspectMatrix);
    esFrustum(&perspectMatrix, -userData->frustumX, userData->frustumX, -userData->frustumY, userData->frustumY, 6, 30);

    if (userData->drawearth && userData->drawuniverse && 1) {
//	userData->drawuniverse(userData, userData->universe_idx, &perspectMatrix);
	glClear(GL_DEPTH_BUFFER_BIT);
	userData->drawearth(userData, userData->earth_idx, &perspectMatrix);
    } else {
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

	    // load the normal data
	    std::vector<float> normals= mesh.normals;
	    glVertexAttribPointer(userData->normalLoc, 3, GL_FLOAT,   GL_FALSE, 0,  &normals.front());
	    glEnableVertexAttribArray(userData->normalLoc);
	
	    // load texCoord data
	    glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, & mesh.texcoords.front());
	    glEnableVertexAttribArray(userData->texCoordLoc);

	    // load the MVP matrix
	    glUniformMatrix4fv(
		glGetUniformLocation(userData->programObject, "uModelMatrix"), 1, false, (GLfloat*)&modelMatrix);
	    glUniformMatrix4fv(
		glGetUniformLocation(userData->programObject, "uProjectionMatrix"), 1, false, (GLfloat*)&perspectMatrix);
	    glUniform1f(
		glGetUniformLocation(userData->programObject, "u_useLight"),
		userData->speeds->at(i)->use_light);
	
	    /// load the texture
	    // Bind the texture
	    glActiveTexture ( GL_TEXTURE0 );
	    glBindTexture ( GL_TEXTURE_2D, userData->texure );
	
	    // Set the sampler texture unit to 0
	    glUniform1i ( userData->textUniformLoc, 0 );

	    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &( mesh.indices.front()));
	}
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

    int width = 400;
    int height = 400;
    // int width = 1184;
    // int height = 768;

    esCreateWindow(&esContext, "Earth_Universe", width, height, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    userData.frustumX = 1.0 * width / kFrustumNormal;
    userData.frustumY = 1.0 * height / kFrustumNormal;

    if (!Init(&esContext)) {
	return 0;
    }
    esRegisterDrawFunc(&esContext, Draw);
    esRegisterResizeFunc(&esContext, onSizeChange);
    
    esMainLoop(&esContext);
    
    return 0;
}

}
#endif
