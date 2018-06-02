#version 430
uniform sampler2D texture_0;
out vec4 FragColor;
in vec2 texcoord_0;
in vec2 texcoord_1;

flat in float TTL;

void main () {
	vec4 color1 = texture(texture_0, texcoord_0);
	vec4 color2 = texture(texture_0, texcoord_1);

    float lower_level =  floor(TTL * 2403 / 288)      * 0.125;
    float upper_level = (floor(TTL * 2403 / 288) + 1) * 0.125;
    float value = (TTL - lower_level) / (upper_level - lower_level);

	//color1.w *= value;
	vec4 color = mix(color1, color2, value);

	color.w *= TTL;

	FragColor = color;
}