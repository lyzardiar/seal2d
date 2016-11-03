#ifndef render_opengl_h
#define render_opengl_h

// WC - todo fix
#include "platform.h"

#if defined PLAT_DESKTOP
    #include <OpenGL/gl3.h>


#elif defined PLAT_IOS

    #import <OpenGLES/ES2/glext.h>

    #define glBindVertexArray glBindVertexArrayOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES

#elif defined PLAT_ANDROID

    #import <GLES2/gl2.h>


#endif


#endif
