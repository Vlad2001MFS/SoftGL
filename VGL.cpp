#include "VGL.hpp"
#include "Math.hpp"
#include <vector>

Vec2i gBufferSize = Vec2i(0, 0);
Color *gColorBuffer = nullptr;
float *gDepthBuffer = nullptr;

static std::vector<Color> gColorBufferData;
static std::vector<float> gDepthBufferData;

void vglResizeBuffers(int w, int h) {
    if (gBufferSize.x != w || gBufferSize.y != h) {
        gBufferSize = Vec2i(w, h);

        gColorBufferData.resize(w*h);
        gDepthBufferData.resize(w*h);

        gColorBuffer = gColorBufferData.data();
        gDepthBuffer = gDepthBufferData.data();
    }
}

void vglGetColorBuffer(void *&colorBuffer, int &pitch) {
    colorBuffer = reinterpret_cast<void*>(gColorBuffer);
    pitch = gBufferSize.x*sizeof(Color);
}

void vglExecuteAll() {

}
