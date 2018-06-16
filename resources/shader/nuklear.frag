#version 430 core
precision mediump float;

uniform sampler2D Texture;
uniform float screenGamma = 1.0f;

in vec2 Frag_UV;
in vec4 Frag_Color;

out vec4 Out_Color;

void main(){
   Out_Color = Frag_Color * pow(texture(Texture, Frag_UV.st), vec4(1.0/screenGamma));
}