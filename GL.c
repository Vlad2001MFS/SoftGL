#include "GLInternal.h"
#include "Rasterizer.h"

vglGLState *gCurrentState = NULL;

void vglGLStateSetDefault(vglGLState *state) {
    VGL_COLOR_SET(state->clearColor, 0, 0, 0, 255);
    state->clearDepth = 1.0f;

    VGL_RECT_SET4(state->viewport, 0, 0, 0, 0);
    state->caps = 0;
    state->shadeModel = GL_SMOOTH;
    state->depthFunc = GL_LESS;

    state->matrixMode = GL_MODELVIEW;
    state->currentMat = &state->modelViewMat;
    VGL_MAT4_SET_IDENTITY(state->projMat);
    VGL_MAT4_SET_IDENTITY(state->modelViewMat);

    VGL_COLOR_SET(state->imColor, 255, 255, 255, 255);
    state->imQuadVertsCounter = 0;
    state->primType = 0;
}

// ############################################################################################

GLAPI void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
    VGL_COLOR_SET_FLOAT4(gCurrentState->clearColor, (uint8_t)red, (uint8_t)green, (uint8_t)blue, (uint8_t)alpha);
}

GLAPI void glClearDepth(GLclampd depth) {
    gCurrentState->clearDepth = (float)depth;
}

GLAPI void glClear(GLbitfield mask) {
    if (mask & GL_COLOR_BUFFER_BIT) {
        vglRSClearColor(&gCurrentState->clearColor);
    }
    if (mask & GL_DEPTH_BUFFER_BIT) {
        vglRSClearDepth(gCurrentState->clearDepth);
    }
}

// ############################################################################################

GLAPI void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    VGL_RECT_SET4_SIZED(gCurrentState->viewport, x, y, width, height);
}

GLAPI void glEnable(GLenum cap) {
    gCurrentState->caps |= cap;
}

GLAPI void glDisable(GLenum cap) {
    gCurrentState->caps &= ~cap;
}

GLAPI void glShadeModel(GLenum mode) {
    gCurrentState->shadeModel = mode;
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
    vglVertex *v = vglVPNewVertex();
    VGL_VEC4_SET(v->pos, x, y, z, w);
    v->color = gCurrentState->imColor;

    if (gCurrentState->primType == GL_QUADS) {
        gCurrentState->imQuadVertsCounter++;
        if (gCurrentState->imQuadVertsCounter == 3) {
            vglVPCopyVertex(vglVPGetVerticesCount() - 3); // Vertex 4
            vglVPCopyVertex(vglVPGetVerticesCount() - 2); // Vertex 5
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
    vglVPProcess();
    gCurrentState->primType = 0;
}
