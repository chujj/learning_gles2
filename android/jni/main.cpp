#include <GLES2/gl2.h>
#include <android/log.h>

int main(int argc, char *argv[])
{
    int program = 0;
    
    program = glCreateProgram();

    // if (!program) {
    // 	glGetProgramiv(
    // }
    
    __android_log_print(android_LogPriority.ANDROID_LOG_ERROR, "zjj-native-tag", "zjj-native-tag");
    
    return 0;
}

