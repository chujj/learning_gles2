#ifndef _EARTH_H_
#define _EARTH_H_

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "read.hpp"

class Earth_Universe_Rotate_Loc_Speed
{
    
public:
    
    Earth_Universe_Rotate_Loc_Speed(GLfloat a_delta_angle, GLfloat a_translate_z,
				    GLfloat a_dx = 0, GLfloat a_dy = 0, GLfloat a_dz = 0,
				    GLfloat a_delta_max = 360.0f)
	: delta_angle(a_delta_angle), translate_z(a_translate_z), dx(a_dx), dy(a_dy), dz(a_dz), delta_max(a_delta_max), use_light(0)
	{}

    inline void nextframe() {
	angle += delta_angle;
	angle = (angle > delta_max ? 0 : angle);
    }
    
    GLfloat dx, dy , dz;
    GLfloat angle;
    GLfloat translate_z;
    GLfloat use_light;
    
private:
    GLfloat delta_angle;
    GLfloat delta_max;
};

typedef struct 
{
    // paramaters from command line
    const char* vert_shader_file;
    const char* frag_shader_file;
    const char* objfile;
    const char* texture_png_file;

    // gles related reference
    GLuint programObject;
    std::vector<tinyobj::shape_t> * shapes;
    std::vector<Earth_Universe_Rotate_Loc_Speed *> * speeds;
    ESMatrix mMVPMatrix;
    GLfloat frustumX, frustumY;
    GLuint texure;
    GLuint positionLoc, normalLoc, texCoordLoc, textUniformLoc;
} UserData;


#ifndef SANSHICHUAN_ANDROID_BUILD
namespace {
int  Init(ESContext *esContext);
void  Draw(ESContext *esContext);
}

#else

int  Init(UserData *userData);
void  Draw(UserData *userData);
void onSizeChange(UserData *userData, int vp_width, int vp_height);
#endif /* SANSHICHUAN_ANDROID_BUILD */


#endif /* _EARTH_H_ */
