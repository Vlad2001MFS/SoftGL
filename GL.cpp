#include "GL.hpp"
#include "Internal.hpp"
#include "Renderer.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <intrin.h>

glm::ivec2 gVPPos = glm::ivec2(0, 0);
glm::ivec2 gVPSize = glm::ivec2(0, 0);

int gMatrixMode = GL_MODELVIEW;
glm::mat4 gProjMat = glm::mat4(1.0f);
glm::mat4 gModelViewMat = glm::mat4(1.0f);

uint32_t gClearColor = 0;
float gClearDepth = 1.0f;

int gImMode = 0;
std::vector<Vertex> gImVerts;
DrawOp *gImROP = nullptr;
glm::u8vec4 gImCurrentColor = glm::u8vec4(255, 255, 255, 255);

__forceinline glm::mat4 &currentMat() {
    if (gMatrixMode == GL_MODELVIEW) {
        return gModelViewMat;
    }
    else if (gMatrixMode == GL_PROJECTION) {
        return gProjMat;
    }
}

__forceinline uint32_t floatColorToUint(float r, float g, float b, float a) {
    return *reinterpret_cast<const uint32_t*>(glm::value_ptr(glm::u8vec4(r*255, g*255, b*255, a*255)));
}

void glViewport(int x, int y, int w, int h) {
    gVPPos = glm::ivec2(x, y);
    gVPSize = glm::ivec2(w, h);

    auto &op = editOrCreateRenderOp(ROPType::Viewport).viewport;
    op.min = glm::max(gVPPos, gSurfaceMin);
    op.max = glm::min(gVPPos + gVPSize - 1, gSurfaceMax);
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
    currentMat() *= glm::rotate(glm::radians(angle), glm::vec3(x, y, z));
}

void glScalef(float x, float y, float z) {
    currentMat() *= glm::scale(glm::vec3(x, y, z));
}

void glClearColor(float r, float g, float b, float a) {
    gClearColor = floatColorToUint(r, g, b, a);
}

void glClearDepth(float depth) {
    gClearDepth = depth;
}

void glClear(int mask) {
    auto &op = editOrCreateRenderOp(ROPType::Clear).clear;
    op.color = gClearColor;
    op.isClearColor = (mask & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT;
    op.depth = gClearDepth;
    op.isClearDepth = (mask & GL_DEPTH_BUFFER_BIT) == GL_DEPTH_BUFFER_BIT;
}

void glEnable(int cap) {
    auto &op = editOrCreateRenderOp(ROPType::Enable).enable;
    op.isDepthTest = (cap & GL_DEPTH_TEST) == GL_DEPTH_TEST;
}

void glDepthFunc(int func) {
    auto &op = editOrCreateRenderOp(ROPType::DepthFunc).depthFunc;
    switch (func) {
        case GL_NEVER: {
            op.func = CompareFunc::Never;
            break;
        }
        case GL_LESS: {
            op.func = CompareFunc::Less;
            break;
        }
        case GL_EQUAL: {
            op.func = CompareFunc::Equal;
            break;
        }
        case GL_LEQUAL: {
            op.func = CompareFunc::LessEqual;
            break;
        }
        case GL_GREATER: {
            op.func = CompareFunc::Greater;
            break;
        }
        case GL_NOTEQUAL: {
            op.func = CompareFunc::NotEqual;
            break;
        }
        case GL_GEQUAL: {
            op.func = CompareFunc::GreaterEqual;
            break;
        }
        case GL_ALWAYS: {
            op.func = CompareFunc::Always;
            break;
        }
    }
}

void glBegin(int mode) {
    gImMode = mode;
    gImVerts.clear();

    PrimType primType;
    switch (mode) {
        case GL_TRIANGLES: {
            primType = PrimType::Triangles;
            break;
        }
        case GL_QUADS: {
            primType = PrimType::Triangles;
            break;
        }
    }
    gImROP = &editOrCreateRenderOp(ROPType::Draw, [&](const RenderOp &rop) { return rop.draw.primType == primType; }).draw;
    gImROP->primType = primType;
}

void glVertex3f(float x, float y, float z) {
    Vertex v;
    v.pos = glm::vec3(x, y, z);
    v.color = gImCurrentColor;
    gImVerts.push_back(v);
}

void glColor3f(float r, float g, float b) {
    gImCurrentColor = glm::u8vec4(r*255, g*255, b*255, 1.0f);
}

void glEnd() {
    glm::mat4 mvp = gProjMat*gModelViewMat;
    for (auto &vert : gImVerts) {
        glm::vec4 v = mvp*glm::vec4(vert.pos, 1.0f);
        vert.pos = glm::vec3(v) / v.w;
        vert.pos = (vert.pos + 1.0f) / 2.0f;
        vert.pos.y = 1.0f - vert.pos.y;
        vert.pos.x = static_cast<int>(gVPPos.x + vert.pos.x*gVPSize.x);
        vert.pos.y = static_cast<int>(gVPPos.y + vert.pos.y*gVPSize.y);
    }

    if (gImMode == GL_TRIANGLES) {
        gImROP->verts = gImVerts;
    }
    else if (gImMode == GL_QUADS) {
        gImROP->verts.reserve(gImVerts.size()*6 / 4);
        for (int i = 0; i < gImVerts.size(); i += 4) {
            gImROP->verts.emplace_back(std::move(gImVerts[i + 0]));
            gImROP->verts.emplace_back(std::move(gImVerts[i + 1]));
            gImROP->verts.emplace_back(std::move(gImVerts[i + 2]));

            gImROP->verts.emplace_back(std::move(gImVerts[i + 0]));
            gImROP->verts.emplace_back(std::move(gImVerts[i + 2]));
            gImROP->verts.emplace_back(std::move(gImVerts[i + 3]));
        }
    }
}
