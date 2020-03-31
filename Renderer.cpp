#include "Renderer.hpp"
#include "Internal.hpp"
#include "glm/ext.hpp"

using CompareFuncSig = bool(*)(float lhs, float rhs);

CompareFuncSig gCompareFuncsTable[static_cast<size_t>(CompareFunc::_Count)] = {
    [](float lhs, float rhs){ return false;                                                 }, // Never
    [](float lhs, float rhs){ return lhs < rhs;                                             }, // Less
    [](float lhs, float rhs){ return glm::epsilonEqual(lhs, rhs, glm::epsilon<float>());    }, // Equal
    [](float lhs, float rhs){ return lhs <= rhs;                                            }, // LessEqual
    [](float lhs, float rhs){ return lhs > rhs;                                             }, // Greater
    [](float lhs, float rhs){ return glm::epsilonNotEqual(lhs, rhs, glm::epsilon<float>()); }, // NotEqual
    [](float lhs, float rhs){ return lhs >= rhs;                                            }, // GreaterEqual
    [](float lhs, float rhs){ return true;                                                  }, // Always
};

std::vector<RenderOp> gRenderOps;

glm::ivec2 gVPMin = glm::ivec2(0, 0);
glm::ivec2 gVPMax = glm::ivec2(0, 0);

bool gIsDepthTest = false;
CompareFuncSig gDepthFunc = gCompareFuncsTable[static_cast<size_t>(CompareFunc::Less)];

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

    gDepthFunc = gCompareFuncsTable[static_cast<size_t>(desc.func)];
}

void opDraw(const RenderOp &rop) {
    auto &desc = rop.draw;

    for (int i = 0; i < desc.verts.size(); i += 3) {
        const Vertex &A = desc.verts[i + 0];
        const Vertex &B = desc.verts[i + 1];
        const Vertex &C = desc.verts[i + 2];

        const int ABx = B.pos.x - A.pos.x;
        const int ACx = C.pos.x - A.pos.x;

        const int ABy = B.pos.y - A.pos.y;
        const int ACy = C.pos.y - A.pos.y;

        const int w = ABx*ACy - ACx*ABy;
        if (glm::abs(w) >= 1.0f) {
            const float invW = 1.0f / w;

            const glm::ivec2 min = glm::clamp(glm::min<glm::ivec2>(A.pos, B.pos, C.pos), gVPMin, gVPMax);
            const glm::ivec2 max = glm::clamp(glm::max<glm::ivec2>(A.pos, B.pos, C.pos), gVPMin, gVPMax);

            for (int y = min.y; y <= max.y; y++) {
                const int PAy = A.pos.y - y;

                for (int x = min.x; x <= max.x; x++) {
                    const int PAx = A.pos.x - x;

                    const float bcScreenU = (ACx*PAy - PAx*ACy)*invW;
                    const float bcScreenV = (PAx*ABy - ABx*PAy)*invW;
                    const float bcScreenW = 1.0f - bcScreenU - bcScreenV;

                    if (bcScreenU >= 0 && bcScreenV >= 0 && bcScreenW >= 0) {
                        float bcClipU = bcScreenU / B.pos.z;
                        float bcClipV = bcScreenV / C.pos.z;
                        float bcClipW = bcScreenW / A.pos.z;

                        const float bcClipUVW = bcClipU + bcClipV + bcClipW;
                        bcClipU /= bcClipUVW; // perspective-correct
                        bcClipV /= bcClipUVW;
                        bcClipW /= bcClipUVW;

                        const float depth = bcClipU*B.pos.z + bcClipV*C.pos.z + bcClipW*A.pos.z;

                        uint32_t idx = x + y*gBufferSize.x;
                        if (gIsDepthTest && gDepthFunc(depth, gDepthBuffer[idx])) {
                            glm::u8vec4 &color = gColorBuffer[idx];
                            color.r = (bcScreenU*B.color.r + bcScreenV*C.color.r + bcScreenW*A.color.r);
                            color.g = (bcScreenU*B.color.g + bcScreenV*C.color.g + bcScreenW*A.color.g);
                            color.b = (bcScreenU*B.color.b + bcScreenV*C.color.b + bcScreenW*A.color.b);
                            color.a = (bcScreenU*B.color.a + bcScreenV*C.color.a + bcScreenW*A.color.a);

                            gDepthBuffer[idx] = depth;
                        }
                        else if (!gIsDepthTest) {
                            glm::u8vec4 &color = gColorBuffer[idx];
                            color.r = (bcScreenU*B.color.r + bcScreenV*C.color.r + bcScreenW*A.color.r);
                            color.g = (bcScreenU*B.color.g + bcScreenV*C.color.g + bcScreenW*A.color.g);
                            color.b = (bcScreenU*B.color.b + bcScreenV*C.color.b + bcScreenW*A.color.b);
                            color.a = (bcScreenU*B.color.a + bcScreenV*C.color.a + bcScreenW*A.color.a);
                        }
                    }
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
