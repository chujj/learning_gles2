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
    va_list params;
    char buf[512];

    va_start ( params, formatStr );
    vsprintf ( buf, formatStr, params );
    
    LOGE("%s", buf);
    va_end ( params );
}

void * setupGraphics(JNIEnv * env, jstring obj_file, jstring tex_png_file, jstring vert, jstring frag) 
{
    UserData* ud = (UserData *) malloc (sizeof(UserData));
    memset(ud, 0, sizeof(UserData));
    jboolean copy = true;
    
    ud->objfile= env->GetStringUTFChars(obj_file, &copy);
    ud->texture_png_file= env->GetStringUTFChars(tex_png_file, &copy);
    ud->vert_shader_file = env->GetStringUTFChars(vert, &copy);
    ud->frag_shader_file= env->GetStringUTFChars(frag, &copy);

    esLogMessage("vfile0: %s\n", ud->vert_shader_file);
    
    Init(ud);

    return (void*) ud;
}

void renderFrame(jint nativeContext) 
{
    Draw((UserData*) nativeContext);
}

extern "C" {
    JNIEXPORT int JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,
							      jstring obj_file, jstring tex_png_file,
								 jstring vert, jstring frag);

    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_eglChange(JNIEnv *env, jobject obj, jint nativeContext, jint width, jint height);
    
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj, jint nativeContext);
};

JNIEXPORT int JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,
							      jstring obj_file, jstring tex_png_file,
							     jstring vert, jstring frag)
{
    setupGraphics(env, obj_file, tex_png_file, vert, frag);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_eglChange(JNIEnv *env, jobject obj, jint nativeContext, jint width, jint height)
{
    onSizeChange((UserData*)nativeContext, width, height);
}


JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj, jint nativeContext)
{
    renderFrame(nativeContext);
}
