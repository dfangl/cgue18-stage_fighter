#version 330
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vData {
    float TTL;
} vertex[];

out vec2 texcoord_0;
flat out float TTL;

void main (void) {
    const vec2 particle_size = vec2(5, 5);
    vec4 P = gl_in[0].gl_Position;

    // Left-Bottom
    vec2 va = P.xy + vec2(-0.5, -0.5) * particle_size;
    gl_Position = projection * vec4(va, P.zw);
    texcoord_0  = vec2(0.0, 0.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    // Left-Top
    vec2 vb = P.xy + vec2(-0.5,  0.5) * particle_size;
    gl_Position = projection * vec4(vb, P.zw);
    texcoord_0  = vec2(0.0, 1.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    // Right-Bottom
    vec2 vc = P.xy + vec2( 0.5, -0.5) * particle_size;
    gl_Position = projection * vec4(vc, P.zw);
    texcoord_0  = vec2(1.0, 0.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    // Right-Top
    vec2 vd = P.xy + vec2( 0.5,  0.5) * particle_size;
    gl_Position = projection * vec4(vd, P.zw);
    texcoord_0  = vec2(1.0, 1.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    EndPrimitive();
}
