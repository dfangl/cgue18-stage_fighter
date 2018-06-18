#version 430 core

out VS_OUT {
    vec2 texcoord_0;
    vec3 normal_0;
    vec3 FragPos;
} vs_out;

in vec3 position;
in vec2 texcoord_0;
in vec3 normal;

uniform mat4 nModel;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
   gl_Position =  projection * view  * model * vec4(position, 1.0);

   vs_out.FragPos    = vec3(model * vec4(position, 1.0));
   vs_out.texcoord_0 = texcoord_0;
   vs_out.normal_0   = mat3(nModel) * normal;

}
