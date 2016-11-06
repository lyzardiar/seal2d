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


#include "../seal.h"

typedef void* DATA_TYPE;
struct array {
    DATA_TYPE* data;
    size_t n;
    size_t cap;
};

struct array* array_new(int cap) {
    if(cap <= 0) {
        return NULL;
    }
    struct array* a = STRUCT_NEW(array);
    a->n = 0;
    a->cap = cap;

    a->data = (DATA_TYPE*)s_malloc(sizeof(DATA_TYPE) * (a->cap));
    memset(a->data, 0, a->cap);
    return a;
}


struct array* array_copy(struct array* self) {
    struct array* a = STRUCT_NEW(array);
    a->n = self->n;
    a->cap = self->cap;

    a->data = (DATA_TYPE*)s_malloc(sizeof(DATA_TYPE) * (a->cap));

    memcpy(a->data, self->data, self->n * sizeof(DATA_TYPE));


    printf("the origin data is: \n");
    for (int i = 0; i < self->n; ++i) {
        printf("0x%x ", array_at(self, i));
    }

    char info[256] = "";
    printf("\n the other is :\n");
    for (int i = 0; i < self->n; ++i) {
        printf("0x%x ", array_at(a, i));
    }

    for (int i = 0; i < self->n; ++i) {
        struct sprite_frame* f = array_at(a, i);
        sprite_frame_tostring(f, (char*)info);
        printf("frame = %s\n", info);
    }

    return a;
}

void array_free(struct array* a) {
    s_free(a->data);
    s_free(a);
}

void array_push_back(struct array* self, void* data) {
    size_t cap = self->cap;
    if(self->n+1 > cap*3/4) {
        cap *= 2;

        self->data = (DATA_TYPE*)s_realloc(self->data, sizeof(DATA_TYPE) * cap);

        self->cap = cap;
    }
    self->data[self->n++] = data;
}

void array_remove(struct array* self, void* data)
{
    int n = self->n;
    int found = -1;
    for (int i = 0; i < n; ++i) {
        if (array_at(self, i) == data) {
            found = i;
            break;
        }
    }
    if (found >= 0) {
        self->data[found] = NULL;
        // shift the array after we move the element.
        // TODO: could we use mmove to improve?
        for (int i = found; i < n-1; ++i) {
            self->data[i] = self->data[i+1];
        }
        --self->n;
    }
}

void array_set(struct array* self, size_t index, void* data) {
    s_assert(index < self->n);
    self->data[index] = data;
}

void* array_at(struct array* self, size_t index) {
    s_assert(index <= self->n);
    return self->data[index];
}

void array_swap(struct array* self, int i, int j)
{
    s_assert(i < self->n && j < self->n);
    void* tmp = self->data[j];
    self->data[j] = self->data[i];
    self->data[i] = tmp;
}

void array_clear(struct array* self, bool cleanup) {
    if (self->n == 0) {
        return;
    }

    if (cleanup) {
        for (size_t i = 0; i < self->n; ++i) {
            void* p = array_at(self, i);
            s_free(p);
        }
    }

    self->n = 0;
    memset(self->data, 0, self->cap);
}

int array_empty(struct array* self) {
    return self->n == 0;
}

size_t array_size(struct array* self) {
    return self->n;
}

void* array_data(struct array* self) {
    return self->data;
}

void array_debug_print(struct array* self) {
    printf("array: data address = %p, n = %ld, cap = %ld\n", self->data, self->n, self->cap);
    printf("array: data is:\n");
    for (int i = 0; i < self->n; ++i) {
        printf("%p ", self->data[i]);
    }
    printf("\n");
}
