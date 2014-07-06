#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "draw_earth.hpp"

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

#include "esUtil.h"
void ESUTIL_API esLogMessage ( const char *formatStr, ... ) 
{
LOGE("log printer, need to implement");
}

UserData * kUserData;

bool setupGraphics(int w, int h) 
{
    kUserData = (UserData *) malloc (sizeof(UserData));
    memset(kUserData, 0, sizeof(UserData));
    kUserData->vert_shader_file = "/sdcard/earth/vert.vert";
    kUserData->frag_shader_file= "/sdcard/earth/frag.frag";
    kUserData->texture_png_file= "/sdcard/earth/earth_universe.png";
    kUserData->objfile= "/sdcard/earth/earth_universe.obj";
    
    Init(kUserData);
    
}

void renderFrame() 
{
    if (kUserData) {
	Draw(kUserData, 400, 800);
    }
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}
