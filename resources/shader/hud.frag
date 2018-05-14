#version 430 core
uniform float visibility;
uniform sampler2D Texture;

in vec2 texcoord;
out vec4 FragColor;

void main() {
    if(texture(Texture, texcoord).a <= 0.0) {
        discard;
    }

    FragColor = texture(Texture, texcoord);
}