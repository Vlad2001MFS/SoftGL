#define __RS_CONCAT2(a, b, c) a##b##c
#define __RS_CONCAT(a, b, c) __RS_CONCAT2(a, b, c)
#define RS_TRI_FUNC_NAME(type) __RS_CONCAT(drawTriangle, type, RS_TRI_NAME)

void RS_TRI_FUNC_NAME(Barycentric_)(const vglVec2i *vpMin, const vglVec2i *vpMax, const vglVertex *A, const vglVertex *B, const vglVertex *C) {
    vglVec2f AC, AB;
    VGL_VEC2_SUB(AC, C->pos, A->pos);
    VGL_VEC2_SUB(AB, B->pos, A->pos);

    const float bcW = AC.x*AB.y - AB.x*AC.y;
    if (bcW >= 1.0f) { // back-face culling
        const float bcInvW = 1.0f / bcW;

#if RS_TRI_SHADEMODEL_SMOOTH
        const vglVec3f colorBCA[] = {
            VGL_VEC3F(B->color.r, C->color.r, A->color.r),
            VGL_VEC3F(B->color.g, C->color.g, A->color.g),
            VGL_VEC3F(B->color.b, C->color.b, A->color.b),
            VGL_VEC3F(B->color.a, C->color.a, A->color.a),
        };
#endif

#if RS_TRI_TEXTURED
        const vglColor *texture = gCurrentState->texture2d->pixels;
        const vglVec2i textureSize = { gCurrentState->texture2d->width, gCurrentState->texture2d->height };

        const vglVec3f texCoordBCA[] = {
            VGL_VEC3F(B->texCoord.x*textureSize.x, C->texCoord.x*textureSize.x, A->texCoord.x*textureSize.x),
            VGL_VEC3F((1.0f - B->texCoord.y)*textureSize.y, (1.0f - C->texCoord.y)*textureSize.y, (1.0f - A->texCoord.y)*textureSize.y)
        };

        vglVec2i texCoord = { 0, 0 };

        float inv255 = 1.0f / 255.0f;
#endif

        const vglVec2i triMin = VGL_VEC2I_MIN3(A->pos, B->pos, C->pos);
        const vglVec2i triMax = VGL_VEC2I_MAX3(A->pos, B->pos, C->pos);
        const vglVec2i min = VGL_VEC2I_CLAMP(triMin, *vpMin, *vpMax);
        const vglVec2i max = VGL_VEC2I_CLAMP(triMax, *vpMin, *vpMax);

        const vglVec3f invBCAz = { 1.0f / B->pos.w, 1.0f / C->pos.w, 1.0f / A->pos.w };

        const bool isDepthTest = gCurrentState->caps & GL_DEPTH_TEST;
        const uint32_t depthFunc = gCurrentState->depthFunc;

        vglVec3f bcScreen = { 0, 0, 0 };
        vglVec3f bcClip = { 0, 0, 0 };

        vglVec2f PA;
        for (int y = min.y; y <= max.y; ++y) {
            PA.y = A->pos.y - y;

            const uint32_t idx = y*gBufferSize.x + min.x;
            uint8_t *colorBuffer = gColorBuffer + idx;
            float *depthBuffer = gDepthBuffer + idx;

            for (int x = min.x; x <= max.x; ++x, colorBuffer += 4, ++depthBuffer) {
                PA.x = A->pos.x - x;

                bcScreen.x = (PA.x*AC.y - AC.x*PA.y)*bcInvW;
                if (bcScreen.x >= 0.0f) {

                    bcScreen.y = (AB.x*PA.y - PA.x*AB.y)*bcInvW;
                    if (bcScreen.y >= 0.0f) {

                        bcScreen.z = 1.0f - bcScreen.x - bcScreen.y;
                        if (bcScreen.z >= 0.0f) {
                            VGL_VEC3_MUL(bcClip, bcScreen, invBCAz);
                            VGL_VEC3_DIV_SCALAR(bcClip, bcClip, bcClip.x + bcClip.y + bcClip.z); // perspective-correction

                            if (isDepthTest) {
                                const float depth = bcClip.x*B->pos.z + bcClip.y*C->pos.z + bcClip.z*A->pos.z;

                                bool depthTestResult = true;
                                COMPARE_FUNC(depthTestResult, depthFunc, depth, *depthBuffer);
                                if (!depthTestResult) {
                                    continue;
                                }
                                *depthBuffer = depth;
                            }

#if RS_TRI_SHADEMODEL_SMOOTH
                            colorBuffer[0] = (uint8_t)VGL_VEC3_DOT(bcClip, colorBCA[0]);
                            colorBuffer[1] = (uint8_t)VGL_VEC3_DOT(bcClip, colorBCA[1]);
                            colorBuffer[2] = (uint8_t)VGL_VEC3_DOT(bcClip, colorBCA[2]);
                            colorBuffer[3] = (uint8_t)VGL_VEC3_DOT(bcClip, colorBCA[3]);
#elif RS_TRI_SHADEMODEL_FLAT
                            *((vglColor*)colorBuffer) = A->color;
#endif
#if RS_TRI_TEXTURED
                            texCoord.x = ((int)VGL_VEC3_DOT(bcClip, texCoordBCA[0])) % textureSize.x;
                            texCoord.y = ((int)VGL_VEC3_DOT(bcClip, texCoordBCA[1])) % textureSize.y;
                            uint8_t *texel = texture + texCoord.x + texCoord.y*textureSize.x;
                            colorBuffer[0] *= texel[0]*inv255;
                            colorBuffer[1] *= texel[1]*inv255;
                            colorBuffer[2] *= texel[2]*inv255;
                            colorBuffer[3] *= texel[3]*inv255;
#endif
                        }
                    }
                }
            }
        }
    }
}