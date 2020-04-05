#include "VertexProcessor.hpp"
#include "GLInternal.hpp"
#include "Rasterizer.hpp"

static std::vector<Vertex> gVertices;

void vpAddVertex(Vertex &&v) {
    gVertices.emplace_back(std::move(v));
}

void vpProcess() {
    auto vp = FloatRect(gCurrentState->viewport);
    auto vpMat = Mat4f::createViewport(vp.min.x, vp.min.y, vp.getSize().x, vp.getSize().y);
    auto mvp = gCurrentState->projMat*gCurrentState->modelViewMat;
    for (auto &v : gVertices) {
        v.pos = mvp*v.pos;
        v.pos /= v.pos.w;
        v.pos = vpMat*v.pos;
    }

    rsProcess();
    gVertices.clear();
}

const std::vector<Vertex> &vpGetVertices() {
    return gVertices;
}
