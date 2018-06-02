#version 430
uniform sampler2D texture_0;
out vec4 FragColor;
in vec2 texcoord_0;

flat in float TTL;

void main () {
	vec4 color = texture(texture_0, texcoord_0);
	color.w *= TTL;

	FragColor = color;
}