#pragma once

#define GL_COLOR_BUFFER_BIT 0x00004000

#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701

#define GL_FLAT 0x1D00
#define GL_SMOOTH 0x1D01

#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_COLOR_BUFFER_BIT 0x00004000

#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_DEPTH_TEST 0x0B71

#define GL_TRIANGLES 0x0004
#define GL_QUADS 0x0007

void glViewport(int x, int y, int w, int h);

void glMatrixMode(int mode);
void glLoadIdentity();
void glLoadMatrixf(const float *mat);
void glTranslatef(float x, float y, float z);
void glRotatef(float angle, float x, float y, float z);
void glScalef(float x, float y, float z);

void glShadeModel(int mode);

void glClearColor(float r, float g, float b, float a);
void glClearDepth(float depth);
void glClear(int mask);

void glEnable(int cap);
void glDepthFunc(int func);

void glBegin(int mode);
void glVertex3f(float x, float y, float z);
void glColor3f(float r, float g, float b);
void glEnd();
