// axes.cpp


#include "headers.h"
#include "axes.h"


char *Axes::vertShader = "\n\
\n\
#version 300 es\n\
\n\
uniform mat4 MVP;\n\
\n\
layout (location = 0) in vec3 vertPosition;\n\
layout (location = 1) in vec3 vertColour;\n\
\n\
smooth out mediump vec3 colour;\n\
\n\
void main() {\n\
  gl_Position = MVP * vec4( vertPosition, 1.0f );\n\
  colour = vertColour;\n\
}";


char *Axes::fragShader = "\n\
\n\
#version 300 es\n\
\n\
smooth in mediump vec3 colour;\n\
out mediump vec4 outputColour;\n\
\n\
void main() {\n\
  outputColour = vec4( colour, 1.0 );\n\
}";



vec3 Axes::verts[8] = {
  vec3(0,0,0), vec3(1,0,0),    // x
  vec3(0,0,0), vec3(0,1,0),    // y
  vec3(0,0,0), vec3(0,0,1),    // z 
  vec3(0,0,0), vec3(0,0,0)     // filled in later with light direction
};


vec3 Axes::colours[8] = {
  vec3(1,0,0), vec3(1,0,0),	        // x
  vec3(0,1,0), vec3(0,1,0),	        // y
  vec3(0.3,0.3,1), vec3(0.3,0.3,1),     // z
  vec3(0.5,0.5,0), vec3(1,1,0)          // light direction
};



Axes::Axes()

{
  // VAO

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  // Positions

  glGenBuffers( 1, &vertexbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
  glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof(GLfloat), verts, GL_DYNAMIC_DRAW );
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // Colours

  glGenBuffers( 1, &colourbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, colourbuffer );
  glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof(GLfloat), colours, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // Shaders

  program.init( vertShader, fragShader );
}



void Axes::draw( mat4 &MVP, vec3 lightDir )

{
  glDisable( GL_DEPTH_TEST );

  // Add light direction to segments to draw

  verts[6] = 3 * lightDir;
  verts[7] = 10 * lightDir;
  
  glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
  glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof(GLfloat), verts, GL_DYNAMIC_DRAW );

  // Draw

  program.activate();

  glUniformMatrix4fv( glGetUniformLocation( program.id(), "MVP"), 1, GL_TRUE, &MVP[0][0] );

  glLineWidth( 3.0 );

  glBindVertexArray( VAO );
  glDrawArrays( GL_LINES, 0, 8 );

  glLineWidth( 1.0 );


  program.deactivate();
}

