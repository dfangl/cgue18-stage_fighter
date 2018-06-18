#version 430 core



struct Light {
    vec3 position; float _padding1;

    vec3 ambient; float _padding2;
    vec3 diffuse; float _padding3;
    vec3 specular; float _padding4;

    float power; float _padding[3];
};

out vec4 FragColor;

in VS_OUT {
     vec2 texcoord_0;
     vec3 normal_0;
     vec3 FragPos;
} fs_in;

uniform vec3 camera_position;
uniform int texture_count;
uniform sampler2D texture_0;
uniform sampler2D texture_1; // Should be valid Lightmap


uniform int lights;
layout(std430, binding = 0) readonly buffer LightBuffer {
    Light light[];
};

uniform float screenGamma = 1.0f;

void main() {
    FragColor = vec4(0.0, 0.0, 0.0 , 1.0);
}