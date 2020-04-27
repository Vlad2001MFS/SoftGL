#include "GLInternal.h"
#include "Rasterizer.h"
#include "Common.h"

vglGLState *gCurrentState = NULL;

void vglGLStateCreate(vglGLState *state) {
    VGL_COLOR_SET(state->clearColor, 0, 0, 0, 255);
    state->clearDepth = 1.0f;

    VGL_RECT_SET4(state->viewport, 0, 0, 0, 0);
    state->shadeModel = GL_SMOOTH;
    state->isDepthTest = false;
    state->depthFunc = GL_LESS;
    state->isTexture2D = false;
    state->isLighting = false;
    for (int i = 0; i < VGL_ARRAYSIZE(state->light); i++) {
        vglLight *light = state->light + i;

        VGL_VEC4_SET(light->ambient, 0.0f, 0.0f, 0.0f, 1.0f);
        if (i == 0) {
            light->state = true;
            VGL_VEC4_SET(light->diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
            VGL_VEC4_SET(light->specular, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            light->state = false;
            VGL_VEC4_SET(light->diffuse, 0.0f, 0.0f, 0.0f, 1.0f);
            VGL_VEC4_SET(light->specular, 0.0f, 0.0f, 0.0f, 1.0f);
        }
        VGL_VEC4_SET(light->position, 0.0f, 0.0f, 1.0f, 0.0f);
        VGL_VEC3_SET(light->spotDirection, 0.0f, 0.0f, -1.0f);
        light->spotExponent = 0.0f;
        light->spotCutOff = 180.0f;
        light->spotCutOffCos = cosf(VGL_RAD(light->spotCutOff));
        light->constantAtt = 1.0f;
        light->linearAtt = 0.0f;
        light->quadraticAtt = 0.0f;
    }
    VGL_VEC4_SET(state->lightModelAmbient, 0.2f, 0.2f, 0.2f, 1.0f);

    state->matrixMode = GL_MODELVIEW;
    state->currentMat = &state->modelViewMat;
    VGL_MAT4_SET_IDENTITY(state->projMat);
    VGL_MAT4_SET_IDENTITY(state->modelViewMat);

    VGL_COLOR_SET(state->imColor, 255, 255, 255, 255);
    VGL_VEC2_SET(state->imTexCoord, 0.0f, 0.0f);
    VGL_VEC3_SET(state->imNormal, 0.0f, 0.0f, 0.0f);
    state->imQuadVertsCounter = 0;
    state->primType = 0;

    VGL_VECTOR_CREATE(state->textures);
    VGL_VECTOR_APPEND_EMPTY(state->textures);
    state->textures.data[0].target = GL_TEXTURE_2D;
    state->textures.data[0].isValid = true;

    state->texture2d = &state->textures.data[0];
}

void vglGLStateDestroy(vglGLState *state) {
    VGL_VECTOR_DESTROY(state->textures);
}

void stateEnableDisable(GLenum cap, bool state) {
    switch (cap) {
        case GL_DEPTH_TEST: {
            gCurrentState->isDepthTest = state;
            break;
        }
        case GL_TEXTURE_2D: {
            gCurrentState->isTexture2D = state;
            break;
        }
        case GL_LIGHTING: {
            gCurrentState->isLighting = state;
            break;
        }
        case GL_LIGHT0:
        case GL_LIGHT1:
        case GL_LIGHT2:
        case GL_LIGHT3:
        case GL_LIGHT4:
        case GL_LIGHT5:
        case GL_LIGHT6:
        case GL_LIGHT7: {
            gCurrentState->light[cap - GL_LIGHT0].state = state;
            break;
        }
    }
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
    stateEnableDisable(cap, true);
}

GLAPI void glDisable(GLenum cap) {
    stateEnableDisable(cap, false);
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

GLAPI void glTexCoord2f(GLfloat s, GLfloat t) {
    VGL_VEC2_SET(gCurrentState->imTexCoord, s, t);
}

GLAPI void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) {
    VGL_VEC3_SET(gCurrentState->imNormal, nx, ny, nz);
}

GLAPI void glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    glVertex4f(x, y, z, 1.0f);
}

GLAPI void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    vglVertex *v = vglVPNewVertex();
    VGL_VEC4_SET(v->pos, x, y, z, w);
    v->color = gCurrentState->imColor;
    v->texCoord = gCurrentState->imTexCoord;
    v->normal = gCurrentState->imNormal;

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

// ############################################################################################

void glGenTextures(GLsizei n, GLuint *textures) {
    for (int i = 0, j = 0; i < n; i++) {
        vglTexture *tex = textures + i;

        if (!tex->isValid) {
            tex->isValid = true;
            textures[j++] = i;
        }
        if (j == n) {
            break;
        }
    }
}

void glDeleteTextures(GLsizei n, const GLuint *textures) {
    for (int i = 0; i < n; i++) {
        uint32_t id = textures[i];

        if (id > 0) {
            vglTexture *tex = gCurrentState->textures.data + id;
            if (tex->target == GL_TEXTURE_2D && gCurrentState->texture2d == tex) {
                gCurrentState->texture2d = &gCurrentState->textures.data[0];
            }
            free(tex->pixels);
            //tex->isValid = false;
            memset(tex, 0, sizeof(vglTexture));
        }
    }
}

void glBindTexture(GLenum target, GLuint texture) {
    if (target == GL_TEXTURE_2D) {
        gCurrentState->texture2d = gCurrentState->textures.data + texture;
        if (gCurrentState->texture2d->target == 0) {
            gCurrentState->texture2d->target = target;
        }
    }
}

void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {
    if (target == GL_TEXTURE_2D) {
        vglTexture *tex = gCurrentState->texture2d;
        tex->level = level;
        tex->internalFormat = internalformat;
        tex->width = width;
        tex->height = height;
        tex->border = border;
        tex->format = format;
        tex->type = type;
        tex->pixels = malloc(sizeof(vglColor)*width*height);
        if (format == GL_RGB) {
            uint8_t *dst = tex->pixels, *src = pixels;
            for (int i = 0; i < width*height*3; i += 3) {
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = 255;
            }
        }
        else if (format == GL_RGBA) {
            uint32_t *dst = tex->pixels, *src = pixels;
            for (int i = 0; i < width*height; i++) {
                *dst++ = *src++;
            }
        }
    }
}

GLAPI void glLightf(GLenum light, GLenum pname, GLfloat param) {
    switch (pname) {
        case GL_SPOT_EXPONENT:
        case GL_SPOT_CUTOFF:
        case GL_CONSTANT_ATTENUATION:
        case GL_LINEAR_ATTENUATION:
        case GL_QUADRATIC_ATTENUATION: {
            glLightfv(light, pname, &param);
            break;
        }
    }
}

GLAPI void glLightfv(GLenum light, GLenum pname, const GLfloat *params) {
    int idx = light - GL_LIGHT0;
    switch (pname) {
        case GL_AMBIENT: {
            VGL_VEC4_SET(gCurrentState->light[idx].ambient, params[0], params[1], params[2], params[3]);
            break;
        }
        case GL_DIFFUSE: {
            VGL_VEC4_SET(gCurrentState->light[idx].diffuse, params[0], params[1], params[2], params[3]);
            break;
        }
        case GL_SPECULAR: {
            VGL_VEC4_SET(gCurrentState->light[idx].specular, params[0], params[1], params[2], params[3]);
            break;
        }
        case GL_POSITION: {
            vglVec4f tmp = { params[0], params[1], params[2], params[3] };
            VGL_MAT4_MUL_VEC4(gCurrentState->light[idx].position, gCurrentState->modelViewMat, tmp);
            break;
        }
        case GL_SPOT_DIRECTION: {
            vglVec3f tmp = { params[0], params[1], params[2] };
            VGL_MAT3_MUL_VEC3(gCurrentState->light[idx].spotDirection, gCurrentState->modelViewMat, tmp);
            break;
        }
        case GL_SPOT_EXPONENT: {
            gCurrentState->light[idx].spotExponent = params[0];
            break;
        }
        case GL_SPOT_CUTOFF: {
            gCurrentState->light[idx].spotCutOff = params[0];
            gCurrentState->light[idx].spotCutOffCos = cosf(VGL_RAD(params[0]));
            break;
        }
        case GL_CONSTANT_ATTENUATION: {
            gCurrentState->light[idx].constantAtt = params[0];
            break;
        }
        case GL_LINEAR_ATTENUATION: {
            gCurrentState->light[idx].linearAtt = params[0];
            break;
        }
        case GL_QUADRATIC_ATTENUATION: {
            gCurrentState->light[idx].quadraticAtt = params[0];
            break;
        }
    }
}

/*GLAPI void glLightModelf(GLenum pname, GLfloat param) {
    switch (pname) {
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
        case GL_LIGHT_MODEL_TWO_SIDE: {
            glLightModelfv(pname, &param);
            break;
        }
    }
}*/

GLAPI void glLightModelfv(GLenum pname, const GLfloat *params) {
    switch (pname) {
        /*case GL_LIGHT_MODEL_LOCAL_VIEWER: {
            
            break;
        }
        case GL_LIGHT_MODEL_TWO_SIDE: {
            
            break;
        }*/
        case GL_LIGHT_MODEL_AMBIENT: {
            VGL_VEC4_SET(gCurrentState->lightModelAmbient, params[0], params[1], params[2], params[3]);
            break;
        }
    }
}
