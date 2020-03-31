#pragma once
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <functional>

enum class CompareFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,

    _Count
};

enum class PrimType {
    Triangles
};

struct Vertex {
    glm::vec3 pos;
    glm::u8vec4 color;
};

struct Triangle {
    glm::vec3 Apos, Bpos, Cpos;
    glm::u8vec4 Acolor, Bcolor, Ccolor;
    glm::ivec2 AB, AC;
    glm::ivec2 min, max;
    int bcW;
    float bcInvW;
    glm::vec3 invABCz;
};

struct ViewportOp {
    glm::ivec2 min;
    glm::ivec2 max;
};

struct ClearOp {
    uint32_t color;
    bool isClearColor;

    float depth;
    bool isClearDepth;
};

struct EnableOp {
    bool isDepthTest;
};

struct DepthFuncOp {
    CompareFunc func;
};

struct DrawOp {
    PrimType primType;
    std::vector<Triangle> triangles;
    glm::ivec2 min = glm::ivec2(std::numeric_limits<int>::max());
    glm::ivec2 max = glm::ivec2(std::numeric_limits<int>::min());
};

enum class ROPType {
    Viewport,
    Clear,
    Enable,
    DepthFunc,
    Draw,

    _Count
};

struct RenderOp {
    ROPType type;

    union {
        ViewportOp viewport;
        ClearOp clear;
        EnableOp enable;
        DepthFuncOp depthFunc;
    };
    DrawOp draw;
};

void executeRenderOps();

RenderOp &createRenderOp(ROPType type);
RenderOp &editOrCreateRenderOp(ROPType type, const std::function<bool(const RenderOp&)> pred = nullptr);