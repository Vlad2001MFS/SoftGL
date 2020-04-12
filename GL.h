#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLAPI
#   define GLAPI
#endif

#ifndef APIENTRY
#   define APIENTRY
#endif

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

/*************************************************************/

#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_COLOR_BUFFER_BIT               0x00004000

#define GL_DEPTH_TEST                     0x0B71

#define GL_FLAT                           0x1D00
#define GL_SMOOTH                         0x1D01

#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207

#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701

//#define GL_POINTS                         0x0000
//#define GL_LINES                          0x0001
//#define GL_LINE_LOOP                      0x0002
//#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
//#define GL_TRIANGLE_STRIP                 0x0005
//#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
//#define GL_QUAD_STRIP                     0x0008
//#define GL_POLYGON                        0x0009

#define GL_TEXTURE_2D                     0x0DE1

#define GL_UNSIGNED_BYTE                  0x1401

#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908

/*************************************************************/

GLAPI void APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLAPI void APIENTRY glClearDepth (GLclampd depth);
GLAPI void APIENTRY glClear (GLbitfield mask);

GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glEnable (GLenum cap);
GLAPI void APIENTRY glDisable (GLenum cap);
GLAPI void APIENTRY glShadeModel (GLenum mode);
GLAPI void APIENTRY glDepthFunc (GLenum func);

GLAPI void APIENTRY glMatrixMode (GLenum mode);
GLAPI void APIENTRY glLoadIdentity (void);
GLAPI void APIENTRY glLoadMatrixf (const GLfloat *m);
GLAPI void APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z);

GLAPI void APIENTRY glBegin (GLenum mode);
GLAPI void APIENTRY glColor3f (GLfloat red, GLfloat green, GLfloat blue);
GLAPI void APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void APIENTRY glTexCoord2f (GLfloat s, GLfloat t);
GLAPI void APIENTRY glVertex3f (GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI void APIENTRY glEnd (void);

GLAPI void APIENTRY glGenTextures (GLsizei n, GLuint *textures);
GLAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
GLAPI void APIENTRY glBindTexture (GLenum target, GLuint texture);
GLAPI void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);

#ifdef __cplusplus
}
#endif
