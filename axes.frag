// axes.frag

#version 300 es
precision mediump float;

smooth in vec3 colour;
out vec4 outputColour;

void main() {
  outputColour = vec4( colour, 1.0 );
}
