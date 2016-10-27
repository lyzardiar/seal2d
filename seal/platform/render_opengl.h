#ifndef render_opengl_h
#define render_opengl_h

#include "platform.h"

#ifdef PLAT_DESKTOP 
    #include <OpenGL/gl3.h>
#endif

#ifdef PLAT_IOS
    #import <OpenGLES/ES2/glext.h>

    #define glBindVertexArray glBindVertexArrayOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES

#endif



#endif
