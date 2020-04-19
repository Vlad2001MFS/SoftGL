#define __RS_CONCAT2(a, b, c) a##b##c
#define __RS_CONCAT(a, b, c) __RS_CONCAT2(a, b, c)
#define RS_TRI_FUNC_NAME(type) __RS_CONCAT(drawTriangle, type, RS_TRI_NAME)

void RS_TRI_FUNC_NAME(HalfPlane_)(const vglVec2i *vpMin, const vglVec2i *vpMax, const vglVertex *A, const vglVertex *B, const vglVertex *C) {
    const vglVec2i triMin = VGL_VEC2I_MIN3(A->pos, B->pos, C->pos);
    const vglVec2i triMax = VGL_VEC2I_MAX3(A->pos, B->pos, C->pos);
    const vglVec2i min = VGL_VEC2I_CLAMP(triMin, *vpMin, *vpMax);
    const vglVec2i max = VGL_VEC2I_CLAMP(triMax, *vpMin, *vpMax);

    const vglVec3f invABCCamZ = { 1.0f / A->pos.z, 1.0f / B->pos.z, 1.0f / C->pos.z };

    const bool isDepthTest = gCurrentState->isDepthTest;
    const uint32_t depthFunc = gCurrentState->depthFunc;
    const float inv255 = 1.0f / 255.0f;

#if RS_TRI_SHADEMODEL_SMOOTH
    vglVec3f colorABC[] = {
        VGL_VEC3F(A->color.r*inv255, B->color.r*inv255, C->color.r*inv255),
        VGL_VEC3F(A->color.g*inv255, B->color.g*inv255, C->color.g*inv255),
        VGL_VEC3F(A->color.b*inv255, B->color.b*inv255, C->color.b*inv255),
        VGL_VEC3F(A->color.a*inv255, B->color.a*inv255, C->color.a*inv255),
    };
    for (int i = 0; i < 4; i++) {
        VGL_VEC3_MUL(colorABC[i], colorABC[i], invABCCamZ);
    }
#endif

#if RS_TRI_TEXTURED
    const vglColor *texture = gCurrentState->texture2d->pixels;
    const int textureWidth = gCurrentState->texture2d->width;
    const vglVec2i textureMax = { gCurrentState->texture2d->width - 1, gCurrentState->texture2d->height - 1 };
    vglVec3f texCoordABC[] = {
        VGL_VEC3F(A->texCoord.x, B->texCoord.x, C->texCoord.x),
        VGL_VEC3F(A->texCoord.y, B->texCoord.y, C->texCoord.y)
    };
    for (int i = 0; i < 2; i++) {
        VGL_VEC3_MUL(texCoordABC[i], texCoordABC[i], invABCCamZ);
        VGL_VEC3_MUL_SCALAR(texCoordABC[i], texCoordABC[i], textureMax.data[i]);
    }
#endif

    const float invArea = 1.0f / VGL_EDGE_FUNCTION(A->pos, B->pos, C->pos);

    vglVec2f P;
    vglVec3f weights;
    vglVec4f color = { 0, 0, 0, 0 };
    for (int y = min.y; y <= max.y; y++) {
        P.y = y + 0.5f;

        const uint32_t idx = y*gBufferSize.x + min.x;
        uint8_t *colorBuffer = gColorBuffer + idx;
        float *depthBuffer = gDepthBuffer + idx;

        for (int x = min.x; x <= max.x; x++, colorBuffer += 4, ++depthBuffer) {
            P.x = x + 0.5f;

            weights.x = VGL_EDGE_FUNCTION(P, B->pos, C->pos);
            weights.y = VGL_EDGE_FUNCTION(P, C->pos, A->pos);
            weights.z = VGL_EDGE_FUNCTION(P, A->pos, B->pos);

            if (weights.x < 0 || weights.y < 0 || weights.z < 0) {
                continue;
            }

            VGL_VEC2_MUL_SCALAR(weights, weights, invArea);
            weights.z = 1.0f - weights.x - weights.y;

            const float depth = VGL_VEC3_DOT(weights, invABCCamZ);
            if (isDepthTest) {
                bool depthTestResult = true;
                const float oneMinusDepth = 1.0f - depth;
                COMPARE_FUNC(depthTestResult, depthFunc, oneMinusDepth, *depthBuffer);
                if (!depthTestResult) {
                    continue;
                }
                *depthBuffer = oneMinusDepth;
            }

            const float invDepth = 1.0f / depth;
#if RS_TRI_SHADEMODEL_SMOOTH
            color.x = VGL_VEC3_DOT(weights, colorABC[0])*invDepth;
            color.y = VGL_VEC3_DOT(weights, colorABC[1])*invDepth;
            color.z = VGL_VEC3_DOT(weights, colorABC[2])*invDepth;
            color.w = VGL_VEC3_DOT(weights, colorABC[3])*invDepth;
#elif RS_TRI_SHADEMODEL_FLAT
            color.x = A->color.r*inv255;
            color.y = A->color.g*inv255;
            color.z = A->color.b*inv255;
            color.w = A->color.a*inv255;
#endif
#if RS_TRI_TEXTURED
            const int u = ((int)(VGL_VEC3_DOT(weights, texCoordABC[0])*invDepth)) % textureMax.x;
            const int v = ((int)(VGL_VEC3_DOT(weights, texCoordABC[1])*invDepth)) % textureMax.y;
            const uint8_t *texel = texture + u + v*textureWidth;

            color.x *= texel[0];
            color.y *= texel[1];
            color.z *= texel[2];
            color.w *= texel[3];
#else
            VGL_VEC4_MUL_SCALAR(color, color, 255);
#endif
            colorBuffer[0] = color.x;
            colorBuffer[1] = color.y;
            colorBuffer[2] = color.z;
            colorBuffer[3] = color.w;
        }
    }
}