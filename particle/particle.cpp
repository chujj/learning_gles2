#include "esUtil.h"
#include "read.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



namespace {
const static float kFrustumNormal = 500;

#define NUM_PARTICLE 360
#define M_PI 3.1415926
#define degreesToRadians(x) (M_PI * x / 180.0)

typedef struct _Particle
{
    float theta;
}Particle;

typedef struct _Emitter
{
    Particle particles[NUM_PARTICLE];
    float k;
} Emitter;

typedef struct _UserData 
{
    GLuint programObject;
    GLuint attri_aTheta_loc;
    GLuint uni_uK_loc, uni_uProjectionMatrix_loc;
    
    float frustumX, frustumY;
    Emitter * emitter;
} UserData;

int Init(ESContext *esContext)
{
    glClearColor(0.3, 0.74, 0.2, 1);

    UserData *userData = (UserData *) esContext->userData;
    Emitter *e = new Emitter;
    e->k = 4.0;
    for (int i = 0; i < NUM_PARTICLE; ++i) {
	e->particles[i].theta = degreesToRadians(i);
    }
    userData->emitter = e;

    // build gl program
    char * vert_shader_str;
    char * frag_shader_str;
    sanshichuan::readFileInStr("particle/vert.vert", &vert_shader_str);
    sanshichuan::readFileInStr("particle/frag.frag", &frag_shader_str);

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

    userData->programObject = programObject;
    // Get the attribute locations
    userData->attri_aTheta_loc = glGetAttribLocation ( userData->programObject, "aTheta" );
    // Get Uniform loc 
    userData->uni_uK_loc = glGetUniformLocation(userData->programObject, "uK");
    userData->uni_uProjectionMatrix_loc = glGetUniformLocation(userData->programObject, "uProjectionMatrix");

    printf("%d, %d, %d\n", userData->attri_aTheta_loc, userData->uni_uK_loc, userData->uni_uProjectionMatrix_loc);
    
    return True;
}

void onSizeChange(ESContext *esContext)
{
    ((UserData*) esContext->userData)->frustumX = 1.0f * esContext->width / kFrustumNormal;
    ((UserData*) esContext->userData)->frustumY = 1.0f * esContext->height / kFrustumNormal;
    glViewport(0, 0, esContext->width, esContext->height);
}

void Draw(ESContext *esContext)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UserData * ud = (UserData*)esContext->userData;
    
    ESMatrix mvpMatrix;
    esMatrixLoadIdentity(&mvpMatrix);
    esFrustum(&mvpMatrix, -ud->frustumX, ud->frustumX, -ud->frustumY, ud->frustumY, 3, 4);

    glUseProgram(ud->programObject);

    glUniformMatrix4fv(ud->uni_uProjectionMatrix_loc, 1, false, (GLfloat *)&mvpMatrix);
    glUniform1f(ud->uni_uK_loc, ud->emitter->k);

    glVertexAttribPointer(ud->attri_aTheta_loc, 1, GL_FLOAT, GL_FALSE, 0, & (ud->emitter->particles));
    glEnableVertexAttribArray(ud->attri_aTheta_loc);
    
    glDrawArrays(GL_POINTS, 0, NUM_PARTICLE);
}

} // namespace ""

extern "C" {
int main(int argc, char *argv[])
{
    ESContext esContext;
    UserData userData;
    memset(&userData, 0, sizeof(UserData));
    
    esInitContext(&esContext);
    esContext.userData = &userData;

    int width = 400;
    int height =400;
    esCreateWindow(&esContext, "Particle", width, height, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
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


