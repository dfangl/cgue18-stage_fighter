#version 430
layout (location = 0) in vec4 position;
layout (location = 1) in float maxTTL;

in mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vData {
    float TTL;
} vertex;

void main () {
  gl_Position  = view * model * vec4(position.xyz, 1.0);
  vertex.TTL   = position.w / maxTTL;
}