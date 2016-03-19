#include <stdio.h>
#include "memory.h"
#include "test.h"

void test_2d_array() {
    int* a = s_malloc(5*3*sizeof(int));
    int* old = a;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            a[j] = i*j;
        }
        a += 5;
    }
    
//    for (int i = 0; i < 3; ++i) {
//        for (int j = 0; j < 5; ++j) {
//            printf("%d ",(&(a[i]))[j]);
//        }
//        printf("\n");
//    }
//    
    for (int i = 0; i < 15; ++i) {
        printf("%d ", old[i]);
    }
    
//    for (int i = 0; i < 3; ++i) {
//        for (int j = 0; j < 5; ++j) {
//            a[]
//        }
//    }

}