
#include "android/android_gl.h"

PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT = 0;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT = 0;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT = 0;

void init_extensions() {
    glGenVertexArraysOESEXT = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
    glBindVertexArrayOESEXT = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
    glDeleteVertexArraysOESEXT = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
}


void gl_helper_init()
{
    init_extensions();

}