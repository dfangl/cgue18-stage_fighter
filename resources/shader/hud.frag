#version 430 core
uniform float visibility;
uniform sampler2D Texture;
uniform float screenGamma = 1.0f;

in vec2 texcoord;
out vec4 FragColor;

void main() {
    if(texture(Texture, texcoord).a <= 0.0) {
        discard;
    }

    FragColor = pow(texture(Texture, texcoord), vec4(1.0/screenGamma));
}