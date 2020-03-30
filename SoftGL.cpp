#include "SoftGL.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <intrin.h>

// ***********************************************************************************************************
// ****************************************** SoftGL System Wrapper ******************************************
// ***********************************************************************************************************

struct {
    glm::ivec2 bufferSize;
    int colorBufferPitch;
    std::vector<uint32_t> colorBuffer;
    std::vector<float> depthBuffer;
} gSGL;

uint32_t *gColorBuffer = nullptr;
float *gDepthBuffer = nullptr;

void sglUpdateBuffers(int w, int h) {
    gSGL.bufferSize = glm::ivec2(w, h);
    gSGL.colorBufferPitch = sizeof(uint32_t)*w;
    gSGL.colorBuffer.resize(w*h);
    gColorBuffer = gSGL.colorBuffer.data();

    gSGL.depthBuffer.resize(w*h);
    gDepthBuffer = gSGL.depthBuffer.data();
}

void sglGetColorBuffer(uint32_t **colorBuffer, int *pitch) {
    *colorBuffer = gColorBuffer;
    *pitch = gSGL.colorBufferPitch;
}

// ***********************************************************************************************************
// ****************************************** SoftGL Implementation ******************************************
// ***********************************************************************************************************

struct {
    glm::ivec2 pos = glm::ivec2(0, 0);
    glm::ivec2 size = glm::ivec2(0, 0);
} gVP;

int gMatrixMode = GL_MODELVIEW;
glm::mat4 gProjMat = glm::mat4(1.0f);
glm::mat4 gModelViewMat = glm::mat4(1.0f);

int gShadeModel = GL_SMOOTH;

uint32_t gClearColor = 0;
float gClearDepth = 1.0f;

bool gIsDepthTestEnabled = false;
int gDepthFunc = GL_LESS;

int gImMode = 0;
std::vector<glm::vec3> gImVertices;

glm::mat4 &currentMat() {
    if (gMatrixMode == GL_MODELVIEW) {
        return gModelViewMat;
    }
    else if (gMatrixMode == GL_PROJECTION) {
        return gProjMat;
    }
}

uint32_t floatColorToUint(float r, float g, float b, float a) {
    return *reinterpret_cast<const uint32_t*>(glm::value_ptr(glm::u8vec4(r*255, g*255, b*255, a*255)));
}

void glViewport(int x, int y, int w, int h) {
    gVP.pos.x = x;
    gVP.pos.y = y;
    gVP.size.x = w;
    gVP.size.y = h;
}

void glMatrixMode(int mode) {
    gMatrixMode = mode;
}

void glLoadIdentity() {
    currentMat() = glm::mat4(1.0f);
}

void glLoadMatrixf(const float *mat) {
    memcpy(glm::value_ptr(currentMat()), mat, sizeof(glm::mat4));
}

void glTranslatef(float x, float y, float z) {
    currentMat() *= glm::translate(glm::vec3(x, y, z));
}

void glRotatef(float angle, float x, float y, float z) {
    currentMat() *= glm::rotate(angle, glm::vec3(x, y, z));
}

void glScalef(float x, float y, float z) {
    currentMat() *= glm::scale(glm::vec3(x, y, z));
}

void glShadeModel(int mode) {
    gShadeModel = mode;
}

void glClearColor(float r, float g, float b, float a) {
    gClearColor = floatColorToUint(r, g, b, a);
}

void glClearDepth(float depth) {
    gClearDepth = depth;
}

void glClear(int mask) {
    if ((mask & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT) {
        for (int y = 0; y < gSGL.bufferSize.y; y++) {
            __stosd(reinterpret_cast<unsigned long*>(gColorBuffer + y*gSGL.bufferSize.x), gClearColor, gSGL.bufferSize.x);
        }
    }
    if ((mask & GL_DEPTH_BUFFER_BIT) == GL_DEPTH_BUFFER_BIT) {
        for (int y = 0; y < gSGL.bufferSize.y; y++) {
            __stosd(reinterpret_cast<unsigned long*>(gDepthBuffer + y*gSGL.bufferSize.x), gClearDepth, gSGL.bufferSize.x);
        }
    }
}

void glEnable(int cap) {
    if ((cap & GL_DEPTH_TEST) == GL_DEPTH_TEST) {
        gIsDepthTestEnabled = true;
    }
}

void glDepthFunc(int func) {
    gDepthFunc = func;
}

void glBegin(int mode) {
    gImMode = mode;
    gImVertices.clear();
}

void glVertex3f(float x, float y, float z) {
    gImVertices.push_back(glm::vec3(x, y, z));
}

void glEnd() {
    glm::mat4 mvp = gProjMat*gModelViewMat;
    for (auto &vert : gImVertices) {
        glm::vec4 v = mvp*glm::vec4(vert, 1.0f);
        vert = glm::vec3(v) / v.w;
        vert = (vert + 1.0f) / 2.0f;
        vert.y = 1.0f - vert.y;
        vert.x = static_cast<int>(gVP.pos.x + vert.x*gVP.size.x);
        vert.y = static_cast<int>(gVP.pos.y + vert.y*gVP.size.y);
    }

    if (gImMode == GL_TRIANGLES) {
        for (int i = 0; i < gImVertices.size(); i += 3) {
            glm::ivec2 min = gImVertices[i + 0];
            glm::ivec2 max = gImVertices[i + 0];
            for (int j = 0; j < 3; j++) {
                const glm::ivec3 &v = gImVertices[i + j];
                for (int k = 0; k < 2; k++) {
                    min[k] = glm::min(min[k], v[k]);
                    max[k] = glm::max(max[k], v[k]);
                }
            }
            min = glm::clamp(min, glm::max(glm::ivec2(0, 0), gVP.pos), glm::min(gSGL.bufferSize, gVP.pos + (gVP.size - 1)));
            max = glm::clamp(max, glm::max(glm::ivec2(0, 0), gVP.pos), glm::min(gSGL.bufferSize, gVP.pos + (gVP.size - 1)));

            const glm::ivec3 &A = gImVertices[i + 0];
            const glm::ivec3 &B = gImVertices[i + 1];
            const glm::ivec3 &C = gImVertices[i + 2];
            for (int y = min.y; y <= max.y; y++) {
                for (int x = min.x; x <= max.x; x++) {
                    const glm::ivec2 P = glm::ivec2(x, y);
                    glm::vec3 uv = glm::cross(glm::vec3(B.x - A.x, C.x - A.x, A.x - P.x), glm::vec3(B.y - A.y, C.y - A.y, A.y - P.y));
                    uv = uv / uv.z;
                    if (uv.x >= 0 && uv.y >= 0 && uv.z >= 0) {
                        gColorBuffer[P.x + P.y*gSGL.bufferSize.x] = floatColorToUint(1.0f, 1.0f, 1.0f, 1.0f);
                    }
                }
            }
        }
    }
}
