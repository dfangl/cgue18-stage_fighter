#version 430 core

#define CEL_LIGHT_LEVELS 2

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

    float power;
};

out vec4 FragColor;

in VS_OUT {
     vec2 texcoord_0;
     vec3 normal_0;
     vec3 FragPos;
} fs_in;

uniform vec3 camera_position;
uniform sampler2D texture_0;    // Texture R
uniform sampler2D texture_1;    // Texture G
uniform sampler2D texture_2;    // Texture B
uniform sampler2D texture_3;    // Texture A
uniform sampler2D texture_4;    // Splatmap
uniform sampler2D texture_5;    // Lightmap

uniform vec4 textureScale = vec4(12, 32, 32, 1);

uniform Material material;
uniform Light light;

uniform float screenGamma = 1.0f;

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

    // Cel stuff:
    //float level = floor(sDotN * CEL_LIGHT_LEVELS);
    //float celBrightness = level / CEL_LIGHT_LEVELS;

    // Calculate acctual light values from diffuse / specular maps
    vec3 diffuseLight  = light.diffuse  * light.power / lDist;
    vec3 specularLight = light.specular * light.power / lDist;

    vec4 alpha = texture2D(texture_4, fs_in.texcoord_0);
    vec4 tex0  = texture2D(texture_0, fs_in.texcoord_0 * textureScale.x);
    vec4 tex1  = texture2D(texture_1, fs_in.texcoord_0 * textureScale.y);
    vec4 tex2  = texture2D(texture_2, fs_in.texcoord_0 * textureScale.z);
    vec4 tex3  = texture2D(texture_3, fs_in.texcoord_0 * textureScale.w);

    //tex0 *= alpha.r;
    //tex1  = mix(tex0, tex1, alpha.g);
    //tex2  = mix(tex1, tex2, alpha.g);
    vec4 texColor =
        alpha.r * tex0 +
        alpha.g * tex1 +
        alpha.b * tex2 ;
        //alpha.a * tex4 ;//mix(tex2, tex3, alpha.a);//tex1;

    vec3 ambient  = light.ambient * vec3(texColor);
    vec3 diffuse  = diffuseLight  * vec3(texColor) * sDotN;// * celBrightness;
    vec3 specular = specularLight * vec3(texColor) * spec ;// * celBrightness;

    vec3 color = ambient + diffuse + specular;
    color = color * texture2D(texture_5, fs_in.texcoord_0).xyz;

    color = pow(color, vec3(1.0/screenGamma));
    FragColor = vec4(color, 1.0);
}