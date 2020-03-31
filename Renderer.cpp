#include "Renderer.hpp"
#include "Internal.hpp"
#include "glm/ext.hpp"
#include <algorithm>

std::vector<RenderOp> gRenderOps;

glm::ivec2 gVPMin = glm::ivec2(0, 0);
glm::ivec2 gVPMax = glm::ivec2(0, 0);

bool gIsDepthTest = false;
CompareFunc gDepthFunc = CompareFunc::Less;

template<typename T>
__forceinline bool compareFunc(CompareFunc func, T lhs, T rhs) {
    switch (func) {
        case CompareFunc::Never: {
            return false;
        }
        case CompareFunc::Less: {
            return lhs < rhs;
        }
        case CompareFunc::Equal: {
            return glm::epsilonEqual(lhs, rhs, glm::epsilon<float>());
        }
        case CompareFunc::LessEqual: {
            return lhs <= rhs;
        }
        case CompareFunc::Greater: {
            return lhs > rhs;
        }
        case CompareFunc::NotEqual: {
            return glm::epsilonNotEqual(lhs, rhs, glm::epsilon<float>());
        }
        case CompareFunc::GreaterEqual: {
            return lhs >= rhs;
        }
        case CompareFunc::Always: {
            return true;
        }
    }
}

void opViewport(const RenderOp &rop) {
    auto &desc = rop.viewport;

    gVPMin = desc.min;
    gVPMax = desc.max;
}

void opClear(const RenderOp &rop) {
    auto &desc = rop.clear;

    if (desc.isClearColor) {
        for (int y = 0; y < gBufferSize.y; y++) {
            __stosd(reinterpret_cast<unsigned long*>(gColorBuffer + y*gBufferSize.x), desc.color, gBufferSize.x);
        }
    }
    if (desc.isClearDepth) {
        for (int i = 0; i < gBufferSize.x*gBufferSize.y; i++) {
            gDepthBuffer[i] = desc.depth;
        }
    }
}

void opEnable(const RenderOp &rop) {
    auto &desc = rop.enable;

    gIsDepthTest |= desc.isDepthTest;
}

void opDepthFunc(const RenderOp &rop) {
    auto &desc = rop.depthFunc;

    gDepthFunc = desc.func;
}

void opDraw(const RenderOp &rop) {
    const auto &desc = rop.draw;

    const glm::ivec2 min = glm::clamp(desc.min, gVPMin, gVPMax);
    const glm::ivec2 max = glm::clamp(desc.max, gVPMin, gVPMax);

    for (int y = min.y; y <= max.y; y++) {
        for (const auto &tri : desc.triangles) {
            const int PAy = tri.A.pos.y - y;

            const int minX = glm::clamp(tri.min.x, gVPMin.x, gVPMax.x);
            const int maxX = glm::clamp(tri.max.x, gVPMin.x, gVPMax.x);
            for (int x = minX; x <= maxX; x++) {
                const int PAx = tri.A.pos.x - x;

                const float bcScreenU = (tri.AC.x*PAy - PAx*tri.AC.y)*tri.bcInvW;
                const float bcScreenV = (PAx*tri.AB.y - tri.AB.x*PAy)*tri.bcInvW;
                const float bcScreenW = 1.0f - bcScreenU - bcScreenV;

                if (bcScreenU >= 0 && bcScreenV >= 0 && bcScreenW >= 0) {
                    float bcClipU = bcScreenU*tri.invAz;
                    float bcClipV = bcScreenV*tri.invBz;
                    float bcClipW = bcScreenW*tri.invCz;

                    const float bcClipUVW = 1.0f / (bcClipU + bcClipV + bcClipW);
                    bcClipU *= bcClipUVW; // perspective-correct
                    bcClipV *= bcClipUVW;
                    bcClipW *= bcClipUVW;

                    const float depth = bcClipU*tri.B.pos.z + bcClipV*tri.C.pos.z + bcClipW*tri.A.pos.z;

                    const uint32_t idx = x + y*gBufferSize.x;
                    if (gIsDepthTest) {
                        if (!compareFunc(gDepthFunc, depth, gDepthBuffer[idx])) {
                            continue;
                        }
                        gDepthBuffer[idx] = depth;
                    }

                    gColorBuffer[idx].r = (bcScreenU*tri.B.color.r + bcScreenV*tri.C.color.r + bcScreenW*tri.A.color.r);
                    gColorBuffer[idx].g = (bcScreenU*tri.B.color.g + bcScreenV*tri.C.color.g + bcScreenW*tri.A.color.g);
                    gColorBuffer[idx].b = (bcScreenU*tri.B.color.b + bcScreenV*tri.C.color.b + bcScreenW*tri.A.color.b);
                    gColorBuffer[idx].a = (bcScreenU*tri.B.color.a + bcScreenV*tri.C.color.a + bcScreenW*tri.A.color.a);
                }
            }
        }
    }
}

void(*gRenderOpFuncsTable[static_cast<size_t>(ROPType::_Count)])(const RenderOp &rop) = {
    opViewport,
    opClear,
    opEnable,
    opDepthFunc,
    opDraw,
};

void executeRenderOps() {
    for (const auto &rop : gRenderOps) {
        gRenderOpFuncsTable[static_cast<size_t>(rop.type)](rop);
    }
    gRenderOps.clear();
}

RenderOp &createRenderOp(ROPType type) {
    RenderOp rop;
    rop.type = type;
    gRenderOps.push_back(rop);
    return gRenderOps.back();
}

RenderOp &editOrCreateRenderOp(ROPType type, const std::function<bool(const RenderOp&)> pred) {
    if (!gRenderOps.empty() && gRenderOps.back().type == type && (!pred || pred(gRenderOps.back()))) {
        return gRenderOps.back();
    }
    else {
        return createRenderOp(type);
    }
}
