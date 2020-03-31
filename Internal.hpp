#pragma once
#include "glm/glm.hpp"

extern glm::ivec2 gBufferSize;
extern glm::ivec2 gSurfaceMin;
extern glm::ivec2 gSurfaceMax;
extern int gColorBufferPitch;
extern glm::u8vec4 *gColorBuffer;
extern float *gDepthBuffer;