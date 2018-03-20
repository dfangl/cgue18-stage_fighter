#version 450 core
in vec3 position;
in vec2 texcoord_0;

out vec2 out_texcoord_0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position =  projection * view  * model * vec4(position, 1.0);
   TexCoord = texcoord_0;
}
