#include "Internal.hpp"

glm::ivec2 gBufferSize = glm::ivec2(0, 0);
glm::ivec2 gSurfaceMin = glm::ivec2(0, 0);
glm::ivec2 gSurfaceMax = glm::ivec2(0, 0);
int gColorBufferPitch = 0;
glm::u8vec4 *gColorBuffer = nullptr;
float *gDepthBuffer = nullptr;