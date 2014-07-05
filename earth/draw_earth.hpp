#ifndef _EARTH_H_
#define _EARTH_H_

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "read.hpp"

namespace {
class Earth_Universe_Rotate_Loc_Speed
{
    
public:
    
    Earth_Universe_Rotate_Loc_Speed(GLfloat a_delta_angle, GLfloat a_translate_z,
				    GLfloat a_dx = 0, GLfloat a_dy = 0, GLfloat a_dz = 0,
				    GLfloat a_delta_max = 360.0f)
	: delta_angle(a_delta_angle), translate_z(a_translate_z), dx(a_dx), dy(a_dy), dz(a_dz), delta_max(a_delta_max)
	{}

    inline void nextframe() {
	angle += delta_angle;
	angle = (angle > delta_max ? 0 : angle);
    }
    
    GLfloat dx, dy , dz;
    GLfloat angle;
    GLfloat translate_z;

private:
    GLfloat delta_angle;
    GLfloat delta_max;
};

typedef struct 
{
    // paramaters from command line
    char* vert_shader_file;
    char* frag_shader_file;
    char* objfile;
    char* texture_png_file;

    // gles related reference
    GLuint programObject;
    std::vector<tinyobj::shape_t> * shapes;
    std::vector<Earth_Universe_Rotate_Loc_Speed *> * speeds;
    ESMatrix mMVPMatrix;
    GLuint texure;
    GLuint positionLoc, texCoordLoc, textUniformLoc;
} UserData;


#ifndef SANSHICHUAN_ANDROID_BUILD
int  Init(ESContext *esContext);
void  Draw(ESContext *esContext);

#else
int  Init(UserData *userData);
void  Draw(UserData *userData);

#endif /* ANDROID_BUILD */

}
#endif /* _EARTH_H_ */
