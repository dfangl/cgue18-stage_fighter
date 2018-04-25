#version 450 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    // vec3 diffuse == textue_0
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

// TODO: move to program
Light light = Light(
    vec3(-50.188, -21.7844, 0.0),
    vec3(0.4, 0.4, 0.4),
    vec3(0.5, 0.5, 0.5),
    vec3(0.8, 0.1, 0.1)
);

void main() {
    // Diffuse
    vec3 norm = normalize(fs_in.normal_0);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);


    // Specular
    vec3 viewDir = normalize(camera_position - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess + 100);


    // Calculate acctual light values from diffuse / specular maps
    vec3 ambient = light.ambient * vec3(texture(texture_0, fs_in.texcoord_0));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_0, fs_in.texcoord_0));
    vec3 specular = light.specular * spec *  vec3(texture(texture_0, fs_in.texcoord_0));

    FragColor = vec4(ambient + diffuse + specular, 1.0);// * texture(texture_0, fs_in.texcoord_0);
}