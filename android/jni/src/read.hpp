#ifndef _READ_H_
#define _READ_H_

#include "esUtil.h"
#include <vector>
#include "tiny_obj_loader.h"

namespace sanshichuan {

long readFileInStr(const char * absFilename, char ** data);
bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData);
GLuint loadTexture(int width, int height, GLubyte *image_data);


}



#endif /* _READ_H_ */

