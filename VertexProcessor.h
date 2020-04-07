#pragma once
#include "MathLib.h"

typedef struct vglVertex {
    vglVec4f pos;
    vglColor color;
} vglVertex;

vglVertex *vglVPNewVertex();
void vglVPCopyVertex(size_t idx);
void vglVPProcess();

const vglVertex *vglVPGetVertices();
size_t vglVPGetVerticesCount();