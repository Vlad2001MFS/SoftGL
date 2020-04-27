#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define VGL_ASSERT(expr) assert(expr)
#define VGL_ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))

//#define VGL_FOREACH(data, size, objVar, idxVar, step) \
//    for (size_t idxVar = 0, cond_##idxVar = 1; idxVar < (size); idxVar += (step), cond_##idxVar = 1) \
//        for (objVar = (data) + idxVar; cond_##idxVar == 1; cond_##idxVar = 0)

#define VGL_VECTOR(T) struct { \
    T *data; \
    size_t size, capacity; \
}

#define VGL_VECTOR_CREATE(self) { memset(&self, 0, sizeof(self)); }
#define VGL_VECTOR_DESTROY(self) { free((self).data); memset(&self, 0, sizeof(self)); }
#define VGL_VECTOR_RESERVE(self, count) { \
    if ((count) > (self).capacity) { \
        (self).data = realloc((self).data, (count)*sizeof((self).data[0])); \
        (self).capacity = count; \
    } \
}
#define VGL_VECTOR_RESIZE(self, count) { \
    VGL_VECTOR_RESERVE(self, count); \
    (self).size = count; \
}
#define VGL_VECTOR_APPEND_EMPTY(self) { \
    VGL_VECTOR_RESIZE(self, (self).size + 1); \
    memset((self).data + (self).size - 1, 0, sizeof((self).data[0])); \
}
#define VGL_VECTOR_APPEND(self, item) { \
    VGL_VECTOR_RESIZE(self, (self).size + 1); \
    memcpy((self).data + (self).size - 1, &(item), sizeof(item)); \
}
#define VGL_VECTOR_CLEAR(self) { VGL_VECTOR_RESIZE(self, 0); }
//#define VGL_VECTOR_FOREACH(self, objVar, idxVar, step) VGL_FOREACH((self).data, (self).size, objVar, idxVar, step)
