#include "test.h"
#include "base/array.h"

void test_array() {
    struct array* a = array_new(10);
    for (int i = 0; i < 10; ++i) {
        array_push_back(a, (void*)i);
    }
    array_debug_print(a);
    for (int i = 0; i < 10; ++i) {
        array_push_back(a, (void*)i);
    }
    for (int i = 0; i < 10; ++i) {
        array_push_back(a, (void*)i);
    }
    for (int i = 0; i < 10; ++i) {
        array_push_back(a, (void*)i);
    }
    for (int i = 0; i < 10; ++i) {
        array_push_back(a, (void*)i);
    }
    for (int i = 0; i < 10; ++i) {
        array_push_back(a, (void*)i);
    }
    for (int i = 0; i < 10; ++i) {
        array_push_back(a, (void*)i);
    }
    array_debug_print(a);

}