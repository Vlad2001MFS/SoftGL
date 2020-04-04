#pragma once

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

 #define GL_NEVER                          0x0200
 #define GL_LESS                           0x0201
 #define GL_EQUAL                          0x0202
 #define GL_LEQUAL                         0x0203
 #define GL_GREATER                        0x0204
 #define GL_NOTEQUAL                       0x0205
 #define GL_GEQUAL                         0x0206
 #define GL_ALWAYS                         0x0207

#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_COLOR_BUFFER_BIT               0x00004000

#define GL_DEPTH_TEST                     0x0B71

#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701

/*************************************************************/

GLAPI void APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLAPI void APIENTRY glClearDepth (GLclampd depth);
GLAPI void APIENTRY glClear (GLbitfield mask);

GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glEnable (GLenum cap);
GLAPI void APIENTRY glDisable (GLenum cap);
GLAPI void APIENTRY glDepthFunc (GLenum func);

GLAPI void APIENTRY glMatrixMode (GLenum mode);
GLAPI void APIENTRY glLoadIdentity (void);
GLAPI void APIENTRY glLoadMatrixf (const GLfloat *m);
GLAPI void APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z);

GLAPI void APIENTRY glBegin (GLenum mode);
GLAPI void APIENTRY glVertex3f (GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI void APIENTRY glEnd (void);
