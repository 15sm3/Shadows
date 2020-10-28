// shader.h

#ifndef SHADER_H
#define SHADER_H

#include "linalg.h"


extern GLuint windowWidth, windowHeight;
extern float factor;
extern float fovy;
extern vec3 eyePosition;
extern vec3 lookAt;
extern float worldRadius;
extern bool showAxes;

void glErrorReport( char *where );


#endif
