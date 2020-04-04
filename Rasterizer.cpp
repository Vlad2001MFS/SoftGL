#include "Rasterizer.hpp"
#include <vector>

static IntRect gBufferRect = IntRect(0, 0, 0, 0);
static Color *gColorBuffer = nullptr;
static float *gDepthBuffer = nullptr;

static std::vector<Color> gColorClearData = { Color(0, 0, 0, 0) };
static std::vector<float> gDepthClearData = { 1.0f };

void rsSetFramebuffer(const IntRect &rect, Color *colorBuffer, float *depthBuffer) {
    gBufferRect = rect;
    gColorBuffer = colorBuffer;
    gDepthBuffer = depthBuffer;

    int area = rect.getArea();
    gColorClearData.resize(area, gColorClearData.front());
    gDepthClearData.resize(area, gDepthClearData.front());
}

const IntRect &rsGetFramebufferRect() {
    return gBufferRect;
}

void rsClearColor(const Color &color) {
    if (gColorClearData.front() != color) {
        std::fill(gColorClearData.begin(), gColorClearData.end(), color);
    }
    memcpy(gColorBuffer, gColorClearData.data(), gColorClearData.size()*sizeof(Color));
}

void rsClearDepth(float depth) {
    if (gDepthClearData.front() != depth) {
        std::fill(gDepthClearData.begin(), gDepthClearData.end(), depth);
    }
    memcpy(gDepthBuffer, gDepthClearData.data(), gDepthClearData.size()*sizeof(float));
}
