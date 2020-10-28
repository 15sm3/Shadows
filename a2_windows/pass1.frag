// Pass 1 fragment shader
//
// Store fragment depth in depth gbuffer

#version 300 es
precision mediump float;


layout (location=0) out float shadowTexture; // output float to shadow texture


in float ccsLightDepth; // Incoming depth is in [0,1]


void main()

{
  // Copy ccsLightDepth into shadow texture

  shadowTexture = ccsLightDepth;
}
