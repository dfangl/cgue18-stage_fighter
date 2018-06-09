#version 430
uniform sampler2D texture_0;
out vec4 FragColor;
in vec2 texcoord_0;
in vec2 texcoord_1;

uniform float screenGamma = 1.0f;

flat in float TTL;

void main () {
	vec4 color1 = texture(texture_0, texcoord_0);
	vec4 color2 = texture(texture_0, texcoord_1);

    float lower_level =  floor(TTL * 2403 / 288)      * 0.125;
    float upper_level = (floor(TTL * 2403 / 288) + 1) * 0.125;
    float value = (TTL - lower_level) / (upper_level - lower_level);

	//color1.w *= value;
	vec4 color = mix(color1, color2, value);
	float alpha = color.w *= TTL;

    color = pow(color, vec4(1.0/screenGamma));
	FragColor = vec4(color.r, color.g, color.b, alpha);
}