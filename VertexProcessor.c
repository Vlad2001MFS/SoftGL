#include "VertexProcessor.h"
#include "GLInternal.h"
#include "Rasterizer.h"
#include "Common.h"

static VGL_VECTOR(vglVertex) gVertices;

void vglVPInitialize() {
    VGL_VECTOR_CREATE(gVertices);
}

void vglVPShutdown() {
    VGL_VECTOR_DESTROY(gVertices);
}

vglVertex *vglVPNewVertex() {
    VGL_VECTOR_APPEND_EMPTY(gVertices);
    return gVertices.data + gVertices.size - 1;
}

void vglVPCopyVertex(size_t idx) {
    VGL_ASSERT(idx < gVertices.size);
    VGL_VECTOR_APPEND(gVertices, gVertices.data[idx]);
}

void __fastcall proccessLight(vglLight *light, vglVec3f *ambient, vglVec3f *diffuse, vglVec3f *specular,
                              const vglVec4f *pos, const vglVec3f *normal) {
    vglVec3f lightDir;
    float att;
    if (light->position.w != 0.0f) {
        VGL_VEC3_SUB(lightDir, light->position, *pos);
        float dist = VGL_VEC3_LEN(lightDir);
        att = 1.0f / (light->constantAtt + light->linearAtt*dist + light->quadraticAtt*dist*dist);
    }
    else {
        VGL_VEC3_SET(lightDir, -light->position.x, -light->position.y, -light->position.z);
        att = 1.0f;
    }
    VGL_VEC3_NORMALIZE(lightDir, lightDir);

    float spotFactor = 1.0f;
    if (light->spotCutOff != 180.0f) {
        vglVec3f toVertexDir;
        VGL_VEC3_SUB(toVertexDir, *pos, light->position);
        VGL_VEC3_NORMALIZE(toVertexDir, toVertexDir);

        float dot = VGL_VEC3_DOT(toVertexDir, light->spotDirection);
        if (dot >= light->spotCutOffCos) {
            spotFactor = pow(dot, light->spotExponent);
        }
        else {
            spotFactor = 0.0f;
        }
    }
    
    float lightFactor = att*spotFactor;

    VGL_VEC3_MUL_SCALAR(*ambient, light->ambient, lightFactor);

    float diffIntensity = VGL_MAX(VGL_VEC3_DOT(*normal, lightDir), 0.0f);
    VGL_VEC3_MUL_SCALAR(*diffuse, light->diffuse, lightFactor*diffIntensity);

    float specIntensity = 0.0f;
    if (diffIntensity != 0.0f) {
        vglVec3f h = { lightDir.x, lightDir.y, lightDir.z + 1 };
        float dot = VGL_VEC3_DOT(*normal, h);
        specIntensity = pow(dot, 32.0f); // instead 32.0f must be GL_SPECULAR_EXPONENT, but I dont known what that and how find this constant
    }
    VGL_VEC3_MUL_SCALAR(*specular, light->specular, lightFactor*specIntensity);

    /*VGL_VEC3_ADD(*ambient, *ambient, light->ambient);

    vglVec3f lightDir;
    float att;
    if (light->position.w == 0.0f) {
        VGL_VEC3_SET(lightDir, -light->position.x, -light->position.y, -light->position.z);
        att = 1.0f;
        VGL_VEC3_NORMALIZE(lightDir, lightDir);
    }
    else {
        VGL_VEC3_SUB(lightDir, light->position, *pos);
        float dist = VGL_VEC3_LEN(lightDir);
        att = 1.0f / (light->constantAtt + light->linearAtt*dist + light->quadraticAtt*dist*dist);
        VGL_VEC3_NORMALIZE(lightDir, lightDir);

        if (light->spotCutOff != 180.0f) {
            vglVec3f spotDir;
            VGL_VEC3_NORMALIZE(spotDir, light->spotDirection);
            VGL_VEC3_NEGATIVE(spotDir, spotDir);

            float c = VGL_VEC3_DOT(lightDir, spotDir);
            if (c < light->spotCutOffCos) {
                att *= 0.0f;
            }
            else if (light->spotExponent > 0.0f) {
                att *= pow(c, light->spotExponent);
            }
        }
    }
    if (att > 0.0f) {
        vglVec3f tmp;

        float diffIntensity = VGL_VEC3_DOT(lightDir, *normal)*att;
        if (diffIntensity > 0.0f) {
            VGL_VEC3_MUL_SCALAR(tmp, light->diffuse, diffIntensity);
            VGL_VEC3_ADD(*diffuse, *diffuse, tmp);
        }

        //vglVec3f invLightDir;
        //VGL_VEC3_NEGATIVE(invLightDir, lightDir);

        //vglVec3f reflectedLightDir;
        //VGL_VEC3_REFLECT(reflectedLightDir, invLightDir, *normal);
        vglVec3f h;
        VGL_VEC3_SUB(h, lightDir, *pos);
        float specIntensity = VGL_VEC3_DOT(h, *normal)*att;
        if (specIntensity > 0.0f) {
            VGL_VEC3_MUL_SCALAR(tmp, light->specular, specIntensity);
            VGL_VEC3_ADD(*specular, *specular, tmp);
        }
    }*/
}

void __fastcall processLighting(vglColor *color, const vglVec4f *vPos, const vglVec3f *vNormal, const vglMat4f *normalMat) {
    vglVec4f modelViewPos;
    VGL_MAT4_MUL_VEC4(modelViewPos, gCurrentState->modelViewMat, *vPos);

    vglVec3f ambient = { 0, 0, 0 };
    vglVec3f diffuse = { 0, 0, 0 };
    vglVec3f specular = { 0, 0, 0 };

    vglVec3f normal;
    VGL_MAT3_MUL_VEC3(normal, *normalMat, *vNormal);
    VGL_VEC3_NORMALIZE(normal, normal);

    for (int k = 0; k < VGL_ARRAYSIZE(gCurrentState->light); k++) {
        vglLight *light = gCurrentState->light + k;
        if (light->state) {
            proccessLight(light, &ambient, &diffuse, &specular, &modelViewPos, &normal);
        }
    }

    vglVec3f totalLight = { gCurrentState->lightModelAmbient.x, gCurrentState->lightModelAmbient.y, gCurrentState->lightModelAmbient.z };
    VGL_VEC3_ADD(totalLight, totalLight, ambient);
    VGL_VEC3_ADD(totalLight, totalLight, diffuse);
    VGL_VEC3_ADD(totalLight, totalLight, specular);

    vglVec3f vertColor = { color->r, color->g, color->b };
    VGL_VEC3_MUL(vertColor, vertColor, totalLight);
    color->r = VGL_CLAMP(vertColor.x, 0, 255);
    color->g = VGL_CLAMP(vertColor.y, 0, 255);
    color->b = VGL_CLAMP(vertColor.z, 0, 255);
}

void vglVPProcess() {
    vglVec2i vpPos = gCurrentState->viewport.min;
    vglVec2i vpSize;
    VGL_RECT_GET_SIZE(vpSize, gCurrentState->viewport);

    vglMat4f vpMat;
    VGL_MAT4_SET_VIEWPORT(vpMat, vpPos.x, vpPos.y, vpSize.x, vpSize.y);

    vglMat4f normalMat;
    VGL_MAT4_INVERSE(normalMat, gCurrentState->modelViewMat);
    VGL_MAT4_TRANSPOSE(normalMat, normalMat);

    vglMat4f mvp;
    VGL_MAT4_MUL(mvp, gCurrentState->projMat, gCurrentState->modelViewMat);

    vglVec4f pos[3];
    float z[3];

    const vglVec3f ndcMin = { -1, -1, -1 };
    const vglVec3f ndcMax = {  1,  1,  1 };

    size_t verticesCount = 0;
    vglVertex *v = gVertices.data;
    for (size_t i = 0; i < gVertices.size; i += 3) {
        vglVertex *tri = gVertices.data + i;

        bool triIsVisible = false;
        for (int j = 0; j < 3; j++) {
            VGL_MAT4_MUL_VEC4(pos[j], mvp, tri[j].pos);
            z[j] = pos[j].w;
            VGL_VEC4_DIV_SCALAR(pos[j], pos[j], pos[j].w);

            triIsVisible = triIsVisible || VGL_VEC3_IN_RANGE(pos[j], ndcMin, ndcMax);
        }

        if (triIsVisible) {
            for (int j = 0; j < 3; j++) {
                VGL_MAT4_MUL_VEC4(pos[j], vpMat, pos[j]);
                pos[j].z = z[j];
            }

            if (VGL_EDGE_FUNCTION(pos[0], pos[1], pos[2]) <= 0.0f) {
                continue;
            }

            for (int j = 0; j < 3; j++) {
                if (gCurrentState->isLighting) {
                    processLighting(&tri[j].color, &tri[j].pos, &tri[j].normal, &normalMat);
                }

                tri[j].pos = pos[j];
            }

            if (v != tri) {
                memcpy(v, tri, sizeof(vglVertex)*3);
            }
            v += 3;
            verticesCount += 3;
        }
    }
    VGL_VECTOR_CLEAR(gVertices);

    vglRSProcess(verticesCount);
}

const vglVertex *vglVPGetVertices() {
    return gVertices.data;
}

size_t vglVPGetVerticesCount() {
    return gVertices.size;
}
