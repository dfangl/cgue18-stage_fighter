#version 450 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
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
Light light = Light(
    vec3(0.0, 0.0, 0.0),
    vec3(1.0, 1.0, 1.0),
    vec3(0.916693, 1.0, 0.510899),
    vec3(0.916693, 1.0, 0.510899)
);

void main() {
    // ===== Light Stuff ====
        // Ambient
        vec3 ambient = light.ambient * material.ambient;

        // diffuse
        //vec3 norm = normalize(fs_in.normal_0);
        vec3 lightDir = normalize(light.position - fs_in.FragPos);
        float diff = max(dot(fs_in.normal_0, lightDir), 0.0);
        vec3 diffuse = light.diffuse * (diff * material.diffuse);

        // specular
        vec3 viewDir = normalize(camera_position - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, fs_in.normal_0);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);

        vec3 result = light.ambient + diffuse + specular;
    // result ==> Light

    FragColor = vec4(result, 1.0) * texture(texture_0, fs_in.texcoord_0);
}