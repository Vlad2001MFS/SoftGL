#include "GLInternal.hpp"
#include "Rasterizer.hpp"

GLState *gCurrentState = nullptr;

// ############################################################################################

GLAPI void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
    gCurrentState->clearColor.setFloat4(red, green, blue, alpha);
}

GLAPI void glClearDepth(GLclampd depth) {
    gCurrentState->clearDepth = static_cast<float>(depth);
}

GLAPI void glClear(GLbitfield mask) {
    if (mask & GL_COLOR_BUFFER_BIT) {
        rsClearColor(gCurrentState->clearColor);
    }
    if (mask & GL_DEPTH_BUFFER_BIT) {
        rsClearDepth(gCurrentState->clearDepth);
    }
}

// ############################################################################################

GLAPI void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    gCurrentState->viewport.set(x, y, width, height);
}

GLAPI void glEnable(GLenum cap) {
    gCurrentState->caps |= cap;
}

GLAPI void glDisable(GLenum cap) {
    gCurrentState->caps &= ~cap;
}

GLAPI void glDepthFunc(GLenum func) {
    gCurrentState->depthFunc = func;
}

// ############################################################################################

GLAPI void glMatrixMode(GLenum mode) {
    gCurrentState->matrixMode = mode;
}

GLAPI void glLoadIdentity(void) {
    gCurrentState->currentMat().setIdentity();
}

GLAPI void glLoadMatrixf(const GLfloat *m) {
    gCurrentState->currentMat().set(m);
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

// ############################################################################################

GLAPI void glBegin(GLenum mode) {
    gCurrentState->primType = mode;
}

GLAPI void glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
    gCurrentState->imColor.setFloat4(red, green, blue, 1.0f);
}

GLAPI void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    gCurrentState->imColor.setFloat4(red, green, blue, alpha);
}

GLAPI void glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    glVertex4f(x, y, z, 1.0f);
}

GLAPI void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    Vertex v;
    v.pos.set(x, y, z, 1.0f);
    v.color = gCurrentState->imColor;
    vpAddVertex(std::move(v));

    if (gCurrentState->primType == GL_QUADS) {
        gCurrentState->imQuadVertsCounter++;
        if (gCurrentState->imQuadVertsCounter == 3) {
            size_t vert4Idx = vpGetVertices().size() - 3;
            size_t vert5Idx = vpGetVertices().size() - 1;
            vpAddVertex(Vertex(vpGetVertices()[vert4Idx]));
            vpAddVertex(Vertex(vpGetVertices()[vert5Idx]));
        }
        else if (gCurrentState->imQuadVertsCounter == 4) {
            gCurrentState->imQuadVertsCounter = 0;
        }
    }
}

GLAPI void glEnd(void) {
    if (gCurrentState->primType == GL_QUADS) {
        gCurrentState->primType = GL_TRIANGLES;
    }
    vpProcess();
    gCurrentState->primType = 0;
}
