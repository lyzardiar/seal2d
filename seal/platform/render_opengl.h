#ifndef render_opengl_h
#define render_opengl_h

#include "platform.h"

#ifdef PLAT_DESKTOP 
#ifdef WIN32
#include "GL/glew.h"
#else
    #include <OpenGL/gl3.h>
#endif
#endif

#ifdef PLAT_IOS
    #import <OpenGLES/ES2/glext.h>

    #define glBindVertexArray glBindVertexArrayOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES

#endif



#endif
