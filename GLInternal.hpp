#pragma once

struct GLState;

extern GLState *gCurrentState;

GLState *createGLState();
void destroyGLState(GLState *state);
