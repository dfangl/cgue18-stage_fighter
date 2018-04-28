#version 450 core

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
const float lightPower = 40.0f;
uniform Light light;

const float screenGamma = 1.0f;

void main() {
    vec3 n = normalize(fs_in.normal_0);
    vec3 v = normalize(camera_position - fs_in.FragPos);
    vec3 s = light.position - fs_in.FragPos;
    vec3 r = -normalize(reflect(s, n));

    float lDist = length(s);
    lDist = lDist * lDist;
    s = normalize(s);

    float sDotN = max( dot(s,n), 0.0 );
    float rDotV = max( dot(r,v), 0.0 );
    float spec  = pow( rDotV, 32);          // TODO: How to get this from a PBR Shader Model?

    // Calculate acctual light values from diffuse / specular maps
    vec3 diffuseLight  = light.diffuse  * lightPower / lDist;
    vec3 specularLight = light.specular * lightPower / lDist;

    vec3 ambient  = light.ambient * vec3(texture(texture_0, fs_in.texcoord_0));
    vec3 diffuse  = diffuseLight  * vec3(texture(texture_0, fs_in.texcoord_0)) * sDotN;
    vec3 specular = specularLight * vec3(texture(texture_0, fs_in.texcoord_0)) * spec ;

    vec3 color = ambient + diffuse + specular;
    color = pow(color, vec3(1.0/screenGamma));

    FragColor = vec4(color, 1.0);
}