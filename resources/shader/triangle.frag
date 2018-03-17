#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

layout(location = 0) uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}