#version 450 core
out vec4 FragColor;

in vec2 out_texcoord_0;

uniform sampler2D texture_0;

void main()
{
    FragColor = texture(texture_0, out_texcoord_0);
}