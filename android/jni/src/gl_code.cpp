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

bool setupGraphics(JNIEnv * env, jstring obj_file, jstring tex_png_file, jstring vert, jstring frag,int w, int h) 
{
    kUserData = (UserData *) malloc (sizeof(UserData));
    memset(kUserData, 0, sizeof(UserData));
    jboolean copy = true;
    
    kUserData->objfile= env->GetStringUTFChars(obj_file, &copy);
    kUserData->texture_png_file= env->GetStringUTFChars(tex_png_file, &copy);
    kUserData->vert_shader_file = env->GetStringUTFChars(vert, &copy);
    kUserData->frag_shader_file= env->GetStringUTFChars(frag, &copy);

    Init(kUserData);
    
}

void renderFrame() 
{
    if (kUserData) {
	Draw(kUserData, 480, 800);
    }
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,
							      jstring obj_file, jstring tex_png_file,
							      jstring vert, jstring frag,
								  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,
							      jstring obj_file, jstring tex_png_file,
							      jstring vert, jstring frag,
							      jint width, jint height)
{
    setupGraphics(env, obj_file, tex_png_file, vert, frag, width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}
