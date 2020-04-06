#include "GLInternal.h"
#include "Rasterizer.h"

vglGLState *gCurrentState = NULL;

void vglGLStateSetDefault(vglGLState *state) {
    VGL_COLOR_SET(state->clearColor, 0, 0, 0, 255);
    state->clearDepth = 1.0f;

    state->matrixMode = GL_MODELVIEW;
    state->currentMat = &state->modelViewMat;
    VGL_MAT4_SET_IDENTITY(state->projMat);
    VGL_MAT4_SET_IDENTITY(state->modelViewMat);

    VGL_RECT_SET4(state->viewport, 0, 0, 0, 0);
    state->depthFunc = GL_LESS;
    state->caps = 0;

    VGL_COLOR_SET(state->imColor, 255, 255, 255, 255);
    state->imQuadVertsCounter = 0;
    state->primType = 0;
}

// ############################################################################################

GLAPI void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
    VGL_COLOR_SET_FLOAT4(gCurrentState->clearColor, red, green, blue, alpha);
}

GLAPI void glClearDepth(GLclampd depth) {
    gCurrentState->clearDepth = depth;
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
    VGL_RECT_SET4(gCurrentState->viewport, x, y, width, height);
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
    if (mode == GL_PROJECTION) {
        gCurrentState->currentMat = &gCurrentState->projMat;
    }
    else if (mode == GL_MODELVIEW) {
        gCurrentState->currentMat = &gCurrentState->modelViewMat;
    }
}

GLAPI void glLoadIdentity(void) {
    VGL_MAT4_SET_IDENTITY(*gCurrentState->currentMat);
}

GLAPI void glLoadMatrixf(const GLfloat *m) {
    VGL_MAT4_SETV(*gCurrentState->currentMat, m);
}

GLAPI void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    vglMat4f m, result;
    VGL_MAT4_SET_ROTATE(m, angle, x, y, z);
    VGL_MAT4_MUL(result, *gCurrentState->currentMat, m);
    *gCurrentState->currentMat = result;
}

GLAPI void glScalef(GLfloat x, GLfloat y, GLfloat z) {
    vglMat4f m, result;
    VGL_MAT4_SET_SCALE(m, x, y, z);
    VGL_MAT4_MUL(result, *gCurrentState->currentMat, m);
    *gCurrentState->currentMat = result;
}

GLAPI void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    vglMat4f m, result;
    VGL_MAT4_SET_TRANSLATE(m, x, y, z);
    VGL_MAT4_MUL(result, *gCurrentState->currentMat, m);
    *gCurrentState->currentMat = result;
}

// ############################################################################################

GLAPI void glBegin(GLenum mode) {
    gCurrentState->primType = mode;
}

GLAPI void glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
    VGL_COLOR_SET_FLOAT4(gCurrentState->imColor, red, green, blue, 1.0f);
}

GLAPI void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    VGL_COLOR_SET_FLOAT4(gCurrentState->imColor, red, green, blue, alpha);
}

GLAPI void glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    glVertex4f(x, y, z, 1.0f);
}

GLAPI void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    vglVertex v;
    VGL_VEC4_SET(v.pos, x, y, z, w);
    v.color = gCurrentState->imColor;
    vglVPAddVertex(&v);

    if (gCurrentState->primType == GL_QUADS) {
        gCurrentState->imQuadVertsCounter++;
        if (gCurrentState->imQuadVertsCounter == 3) {
            size_t vert4Idx = vglVPGetVerticesCount() - 3;
            size_t vert5Idx = vglVPGetVerticesCount() - 1;
            vglVPAddVertex(&vglVPGetVertices()[vert4Idx]);
            vglVPAddVertex(&vglVPGetVertices()[vert5Idx]);
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
