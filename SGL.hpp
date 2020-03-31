#pragma once
#include <stdint.h>

void sglUpdateBuffers(int w, int h);
void sglGetColorBuffer(uint32_t **colorBuffer, int *pitch);