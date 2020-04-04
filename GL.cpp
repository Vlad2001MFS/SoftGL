#include "GL.hpp"
#include "GLInternal.hpp"
#include "Rasterizer.hpp"

struct GLState {
    Color clearColor = Color(0, 0, 0, 255);
    float clearDepth = 1.0f;

    uint32_t matrixMode = GL_MODELVIEW;
    Mat4f projMat = Mat4f::Identity;
    Mat4f modelViewMat = Mat4f::Identity;

    IntRect viewport = IntRect(0, 0, 0, 0);

    Mat4f &currentMat() {
        if (matrixMode == GL_PROJECTION) {
            return projMat;
        }
        else if (matrixMode == GL_MODELVIEW) {
            return modelViewMat;
        }
    }
} *gCurrentState = nullptr;

GLState *createGLState() {
    return new GLState();
}

void destroyGLState(GLState *state) {
    delete state;
}

GLAPI void glClear(GLbitfield mask) {
    if (mask & GL_COLOR_BUFFER_BIT) {
        rsClearColor(gCurrentState->clearColor);
    }
    if (mask & GL_DEPTH_BUFFER_BIT) {
        rsClearDepth(gCurrentState->clearDepth);
    }
}

GLAPI void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
    gCurrentState->clearColor.setFloat4(red, green, blue, alpha);
}

GLAPI void glClearDepth(GLclampd depth) {
    gCurrentState->clearDepth = static_cast<float>(depth);
}

GLAPI void glLoadIdentity(void) {
    gCurrentState->currentMat().setIdentity();
}

GLAPI void glLoadMatrixf(const GLfloat *m) {
    gCurrentState->currentMat().set(m);
}

GLAPI void glMatrixMode(GLenum mode) {
    gCurrentState->matrixMode = mode;
}

GLAPI void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    gCurrentState->currentMat().rotate(angle, x, y, z);
}

GLAPI void glScalef(GLfloat x, GLfloat y, GLfloat z) {
    gCurrentState->currentMat().scale(x, y, z);
}

GLAPI void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    gCurrentState->currentMat().translate(x, y, z);
}

GLAPI void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    gCurrentState->viewport.set(x, y, width, height);
}
