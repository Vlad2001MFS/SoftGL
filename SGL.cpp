#include "SGL.hpp"
#include "Internal.hpp"
#include "Renderer.hpp"
#include <vector>

struct {
    std::vector<uint32_t> colorBuffer;
    std::vector<float> depthBuffer;
} gSGL;

void sglUpdateBuffers(int w, int h) {
    gBufferSize = glm::ivec2(w, h);
    gSurfaceMin = glm::ivec2(0, 0);
    gSurfaceMax = glm::ivec2(w - 1, h - 1);
    gColorBufferPitch = sizeof(uint32_t)*w;

    gSGL.colorBuffer.resize(w*h);
    gColorBuffer = reinterpret_cast<glm::u8vec4*>(gSGL.colorBuffer.data());

    gSGL.depthBuffer.resize(w*h);
    gDepthBuffer = gSGL.depthBuffer.data();
}

void sglGetColorBuffer(uint32_t **colorBuffer, int *pitch) {
    *colorBuffer = reinterpret_cast<glm::uint32_t*>(gColorBuffer);
    *pitch = gColorBufferPitch;
}

void sglExecuteAll() {
    executeRenderOps();
}
