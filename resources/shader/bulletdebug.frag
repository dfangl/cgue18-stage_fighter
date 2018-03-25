#version 450 core
out vec4 FragColor;

in vec3 out_color;

void main()
{
    FragColor = vec4(out_color, 1.0);
}