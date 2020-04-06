#pragma once
#include <assert.h>

#define VGL_ASSERT(expr) assert(expr)
#define VGL_ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))