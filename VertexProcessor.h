#pragma once
#include "MathLib.h"

typedef struct vglVertex {
    vglVec4f pos;
    vglColor color;
} vglVertex;

void vglVPAddVertex(const vglVertex *v);
void vglVPProcess();

const vglVertex *vglVPGetVertices();
size_t vglVPGetVerticesCount();