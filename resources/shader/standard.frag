#version 450 core

#define PI 3.1415926535897932384626433832795
#define DielectricSpecular vec3(0.4, 0.4, 0.4)

struct Material {
    vec3 baseColor;
    float metallic;
    float roughness;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in VS_OUT {
     vec2 texcoord_0;
     vec3 normal_0;
     vec3 FragPos;
} fs_in;

uniform vec3 camera_position;
uniform sampler2D texture_0;
uniform Material material;

// TODO: Remove default light if everything works
uniform Light light = Light(
    vec3(-50.188, -21.7844, 0.0),
    vec3(0.8, 0.8, 0.8),
    vec3(0.5, 0.5, 0.5),
    vec3(0.2, 0.2, 0.2)
);

void main() {
    vec3 n = normalize(fs_in.normal_0);
    vec3 v = normalize(camera_position - fs_in.FragPos);
    vec3 s = normalize(-light.position + fs_in.FragPos);
    vec3 r = -normalize(reflect(v, n));

    float sDotN = max( dot(s,n), 0.0 );
    float rDotV = max( dot(r,v), 0.0 );
    float spec  = pow( rDotV, material.roughness);

    // Calculate acctual light values from diffuse / specular maps
    vec3 ambient  = light.ambient  * vec3(texture(texture_0, fs_in.texcoord_0));
    vec3 diffuse  = light.diffuse  * vec3(texture(texture_0, fs_in.texcoord_0)) * sDotN;
    vec3 specular = light.specular * vec3(texture(texture_0, fs_in.texcoord_0)) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}