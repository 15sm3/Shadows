// axes.h
//
// Set up and draw a set of (r,g,b) axes


#ifndef AXES_H
#define AXES_H


#include "linalg.h"
#include "gpuProgram.h"


class Axes {

  GLuint vertexbuffer;
  GLuint colourbuffer;

  static vec3 colours[8];
  static vec3 verts[8];

 public:

  GLuint VAO;
  GPUProgram program;
  static char *vertShader;
  static char *fragShader;

  Axes();
  void draw( mat4 &MVP, vec3 lightDir );
};


#endif
