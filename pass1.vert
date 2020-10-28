// Pass 1 vertex shader
//
// Output vertex depth in the light's CCS

#version 300 es
precision mediump float;


uniform mat4 OCS_to_lightCCS; // Transform OCS into light's coordinate system

layout (location=0) in vec3 vertPosition; // OCS vertex position

out float ccsLightDepth; // output vertex depth in [0,1]


void main()

{
  // Calculate the position of this vertex in the light's CCS.
  // Also store this in gl_Position.

  vec4 ccsLightPos = OCS_to_lightCCS * vec4(vertPosition, 1.0f); // CHANGE THIS
  gl_Position = ccsLightPos;

  // Output the ccsLightDepth in the range [0,1]

  ccsLightDepth = ((ccsLightPos.z/ccsLightPos.w)+ 1.0) *0.5; // CHANGE THIS
}
