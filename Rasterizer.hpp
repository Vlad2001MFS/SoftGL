#pragma once
#include "Math.hpp"

void rsSetFramebuffer(const IntRect &rect, Color *colorBuffer, float *depthBuffer);
const IntRect &rsGetFramebufferRect();

void rsClearColor(const Color &color);
void rsClearDepth(float depth);


