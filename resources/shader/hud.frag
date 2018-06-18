#version 430 core
uniform float visibility = 1.0f;
uniform sampler2D Texture;
uniform float screenGamma = 1.0f;

in vec2 texcoord;
out vec4 FragColor;

void main() {
    if(texture(Texture, texcoord).a <= 0.0) {
        discard;
    }

    vec4 color = texture(Texture, texcoord);
    color.a    = color.a * visibility;
    color.rgb  = pow(color.rgb, vec3(1.0/screenGamma));

    FragColor = color;
}