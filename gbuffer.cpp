/*
    Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "headers.h"
#include "gbuffer.h"
#include "font.h"
#include "shader.h"


GBuffer::GBuffer( unsigned int width, unsigned int height, int nTextures, int *textureTypes, GLuint baseTexUnit )

{
  windowWidth = width;
  windowHeight = height;
  numTextures = nTextures;
  baseTextureUnit = GL_TEXTURE0 + baseTexUnit;

 // Create the FBO

  glGenFramebuffers( 1, &FBO );
  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO );

  // Create the gbuffer textures

  textureIDs = new GLuint[ numTextures ];
  glGenTextures( numTextures, textureIDs );
  glGenTextures( 1, &depthTexture );

  for (int i = 0 ; i < numTextures; i++) {

    glActiveTexture( baseTextureUnit + i );
    glBindTexture( GL_TEXTURE_2D, textureIDs[i] );

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glTexImage2D( GL_TEXTURE_2D, 0, textureTypes[i], windowWidth, windowHeight, 0, GL_RED, GL_FLOAT, NULL );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureIDs[i], 0 );
  }

  // depth

  glActiveTexture( baseTextureUnit + numTextures );
  glBindTexture( GL_TEXTURE_2D, depthTexture );

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );

  glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );

  // Declare the drawBuffers

  GLenum *drawBuffers = new GLenum[numTextures];

  for (int i=0; i<numTextures; i++)
    drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

  glDrawBuffers( numTextures, drawBuffers );

  GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    printf("FB error, status: 0x%x\n", status);
    return;
  }

  // restore default FBO

  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}


GBuffer::~GBuffer()

{
  glDeleteFramebuffers( 1, &FBO );
  glDeleteTextures( numTextures, textureIDs );
  glDeleteTextures( 1, &depthTexture );
}


void GBuffer::BindForWriting()

{
  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO );
}


void GBuffer::BindForReading()

{
  glBindFramebuffer( GL_READ_FRAMEBUFFER, FBO );
}


void GBuffer::BindTexture( int textureNumber, int gbTextureUnit )

{
  glActiveTexture( baseTextureUnit + gbTextureUnit );
  glBindTexture( GL_TEXTURE_2D, textureIDs[ textureNumber ] );
}


void GBuffer::UnbindTexture( int textureNumber, int gbTextureUnit )

{
  glActiveTexture( baseTextureUnit + gbTextureUnit );
  glBindTexture( GL_TEXTURE_2D, 0 );
}


void GBuffer::SetReadBuffer( int textureNumber )

{
  glReadBuffer( GL_COLOR_ATTACHMENT0 + textureNumber );
}


void GBuffer::setDrawBuffers( int numDrawBuffers, int *bufferIDs )

{
  GLenum *drawBuffers = new GLenum[numDrawBuffers];

  for (int i=0; i<numDrawBuffers; i++)
    drawBuffers[i] = GL_COLOR_ATTACHMENT0 + bufferIDs[i];

  glDrawBuffers( numDrawBuffers, drawBuffers );

  delete [] drawBuffers;
}


// Debugging output

void GBuffer::DrawGBuffers( GLFWwindow *window )

{
  // Clear window

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Blit textures onto window

  glBindFramebuffer( GL_READ_FRAMEBUFFER, FBO );

  int dim = (int) ceil(sqrt(numTextures)); // use a dim x dim grid

  float size = 1.0 / (float)dim; // scale factor of one image in the grid

  GLsizei width = (GLsizei) (size * windowWidth);
  GLsizei height = (GLsizei) (size * windowHeight);

  int i = 0;

  for (int r=0; r<dim; r++)
    for (int c=0; c<dim; c++)
      if (i < numTextures) {

	SetReadBuffer( i );
	glBlitFramebuffer( 0, 0, windowWidth, windowHeight,
			   c*width, (dim-r-1)*height, (c+1)*width, (dim-r+1-1)*height,
			   GL_COLOR_BUFFER_BIT, GL_LINEAR);

#ifdef USE_FREETYPE
	char buffer[100];
	sprintf( buffer, "gbuffer %d", i );
	render_text( buffer, c*width+10, (dim-r-1)*height+10, window );
#endif

	i++;
      }
}
