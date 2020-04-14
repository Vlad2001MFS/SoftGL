#include "VertexProcessor.h"
#include "GLInternal.h"
#include "Rasterizer.h"
#include "Common.h"

#define VGL_VP_MAX_VERTICES 40000

static vglVertex gVertices[VGL_VP_MAX_VERTICES];
static size_t gVerticesCount = 0;

vglVertex *vglVPNewVertex() {
    VGL_ASSERT(gVerticesCount < VGL_VP_MAX_VERTICES);

    return gVertices + gVerticesCount++;
}

void vglVPCopyVertex(size_t idx) {
    VGL_ASSERT(gVerticesCount < VGL_VP_MAX_VERTICES);
    VGL_ASSERT(idx < gVerticesCount);

    gVertices[gVerticesCount++] = gVertices[idx];
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

    //vglMat4f mvp;
    //VGL_MAT4_MUL(mvp, gCurrentState->projMat, gCurrentState->modelViewMat);

    vglVec4f pos[3], modelViewPos[3];
    float z[3];

    const vglVec3f ndcMin = { -1, -1, -1 };
    const vglVec3f ndcMax = {  1,  1,  1 };

    size_t verticesCount = 0;
    vglVertex *v = gVertices;
    for (size_t i = 0; i < gVerticesCount; i += 3) {
        vglVertex *tri = gVertices + i;

        bool triIsVisible = false;
        for (int j = 0; j < 3; j++) {
            VGL_MAT4_MUL_VEC4(modelViewPos[j], gCurrentState->modelViewMat, tri[j].pos);
            VGL_MAT4_MUL_VEC4(pos[j], gCurrentState->projMat, modelViewPos[j]);
            z[j] = pos[j].z;
            VGL_VEC4_DIV_SCALAR(pos[j], pos[j], pos[j].w);

            triIsVisible = triIsVisible || VGL_VEC3_IN_RANGE(pos[j], ndcMin, ndcMax);
        }

        if (triIsVisible) {
            for (int j = 0; j < 3; j++) {
                VGL_MAT4_MUL_VEC4(tri[j].pos, vpMat, pos[j]);
                tri[j].pos.w = z[j];

                if (gCurrentState->isLighting) {
                    vglVec3f ambient = { 0, 0, 0 };
                    vglVec3f diffuse = { 0, 0, 0 };
                    vglVec3f specular = { 0, 0, 0 };

                    vglVec3f normal;
                    VGL_MAT3_MUL_VEC3(normal, normalMat, tri[j].normal);
                    VGL_VEC3_NORMALIZE(normal, normal);

                    for (int k = 0; k < VGL_ARRAYSIZE(gCurrentState->light); k++) {
                        vglLight *light = gCurrentState->light + k;
                        if (light->state) {
                            VGL_VEC3_ADD(ambient, ambient, light->ambient);

                            vglVec3f lightDir;
                            float att;
                            if (light->position.w == 0.0f) {
                                VGL_VEC3_SET(lightDir, light->position.x, light->position.y, light->position.z);
                                att = 1.0f;
                                VGL_VEC3_NORMALIZE(lightDir, lightDir);
                            }
                            else {
                                VGL_VEC3_SUB(lightDir, light->position, modelViewPos[j]);
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

                                float diffIntensity = VGL_VEC3_DOT(lightDir, normal)*att;
                                if (diffIntensity > 0.0f) {
                                    VGL_VEC3_MUL_SCALAR(tmp, light->diffuse, diffIntensity);
                                    VGL_VEC3_ADD(diffuse, diffuse, tmp);
                                }

                                vglVec3f invLightDir;
                                VGL_VEC3_NEGATIVE(invLightDir, lightDir);

                                vglVec3f reflectedLightDir;
                                VGL_VEC3_REFLECT(reflectedLightDir, invLightDir, normal);
                                float specIntensity = VGL_VEC3_DOT(invLightDir, normal)*att;
                                if (specIntensity > 0.0f) {
                                    VGL_VEC3_MUL_SCALAR(tmp, light->specular, specIntensity);
                                    VGL_VEC3_ADD(specular, specular, tmp);
                                }
                            }
                        }
                    }

                    vglVec3f totalLight = { 0.0f, 0.0f, 0.0f };
                    VGL_VEC3_ADD(totalLight, totalLight, ambient);
                    VGL_VEC3_ADD(totalLight, totalLight, diffuse);
                    VGL_VEC3_ADD(totalLight, totalLight, specular);

                    vglVec3f vertColor = { tri[j].color.r / 255.0f, tri[j].color.g / 255.0f, tri[j].color.b / 255.0f };
                    VGL_VEC3_MUL(vertColor, vertColor, totalLight);
                    tri[j].color.r = VGL_CLAMP(vertColor.x, 0.0f, 1.0f)*255;
                    tri[j].color.g = VGL_CLAMP(vertColor.y, 0.0f, 1.0f)*255;
                    tri[j].color.b = VGL_CLAMP(vertColor.z, 0.0f, 1.0f)*255;
                }
            }

            if (v != tri) {
                memcpy(v, tri, sizeof(vglVertex)*3);
            }
            v += 3;
            verticesCount += 3;
        }
    }
    gVerticesCount = 0;

    vglRSProcess(verticesCount);
}

const vglVertex *vglVPGetVertices() {
    return gVertices;
}

size_t vglVPGetVerticesCount() {
    return gVerticesCount;
}
