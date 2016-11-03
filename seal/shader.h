/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef __seal__shader__
#define __seal__shader__

#include "seal_base.h"

#define CHECK_GL_ERROR check_gl_error(__FILE__, __LINE__);

enum SHADER_TYPE {
    SHADER_SPRITE = 0,
    SHADER_PRIMITIVE = 1,
    SHADER_SPINE = 2,

    MAX_SHADER = 16,
};

enum UNIFORM_ATTRIBUTE_TYPE {
    UNIFORM_1F = 0,
    UNIFORM_2F,
    UNIFORM_3F,
    UNIFORM_4F,
};

enum BUILT_IN_UNIFORMS {
    BUILT_IN_MIX_COLOR,

    BUILT_IN_UNIFORM_MAX,
};

struct builtin_uniform {
    enum BUILT_IN_UNIFORMS type;
    enum UNIFORM_ATTRIBUTE_TYPE attr_type;
    const char* name;
};

struct uniform_buffer_object {
    enum BUILT_IN_UNIFORMS type;
    float v[4];
};

/* it looks like we don't need this function,
 cause the object is managed by the render's array to clean it up.*/

//void uniform_buffer_object_free(struct uniform_buffer_object* self);

struct shader {
    GLuint shader_programs[MAX_SHADER];
    struct builtin_uniform uniforms[BUILT_IN_UNIFORM_MAX];

    struct uniform_buffer_object uniform_buffer_objects[BUILT_IN_UNIFORM_MAX];
};

void check_gl_error(const char* file, int line);

struct shader* shader_new();
void shader_free(struct shader* self);

// set the pre-allocated uniform object.
void shader_set_uniform_object(struct shader* self,
                               enum BUILT_IN_UNIFORMS type,
                               float* v);

void shader_commit_uniform(struct shader* self, GLint program);
void shader_set_uniform(struct shader* self, GLint program,
                        enum BUILT_IN_UNIFORMS type, void* v);

GLuint shader_get_program(struct shader* self, enum SHADER_TYPE shader_index);


#endif
