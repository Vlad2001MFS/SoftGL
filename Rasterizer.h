#pragma once
#include "MathLib.h"
#include "VertexProcessor.h"

void vglRSSetFramebuffer(const vglIntRect *rect, vglColor *colorBuffer, float *depthBuffer);
const vglIntRect *vglRSGetFramebufferRect();

void vglRSClearColor(const vglColor *color);
void vglRSClearDepth(float depth);

void vglRSProcess(size_t verticesCount);
