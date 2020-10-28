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

#ifndef GBUFFER_H
#define	GBUFFER_H


class GBuffer

{
  GLuint FBO;
  GLuint depthTexture;

  unsigned int windowWidth, windowHeight;

  int numTextures;

  int baseTextureUnit;

 public:

  GLuint *textureIDs;

  GBuffer( unsigned int width, unsigned int height, int nTextures, int *textureTypes, GLuint baseTexUnit );

  ~GBuffer();

  void BindForWriting();
  void BindForReading();  
  void BindTexture( int textureNumber, int gbTextureUnit );
  void UnbindTexture( int textureNumber, int gbTextureUnit );
    
  void SetReadBuffer( int textureNumber );

  void setDrawBuffers( int numDrawBuffers, int *bufferIDs );

  void DrawGBuffers( GLFWwindow *window );
};

#endif	/* SHADOWMAPFBO_H */

