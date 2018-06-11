#version 330
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in mat4 model[];
uniform mat4 view;
uniform mat4 projection;
uniform vec2 particle_size;

in vData {
    float TTL;
} vertex[];

out vec2 texcoord_0;
out vec2 texcoord_1;
flat out float TTL;

// UV Mapping for explosion.png:
float texcoord_explosion(float ttl, int x) {
    return clamp(8 - (floor(ttl * 2403 / 288) + x), 0, 8) * 0.111189;
}

void main (void) {
    vec4 P = gl_in[0].gl_Position;

    // Left-Bottom
    vec2 va = P.xy + vec2(-0.5, -0.5) * particle_size;
    gl_Position = projection * vec4(va, P.zw);
    texcoord_0  = vec2(texcoord_explosion(vertex[0].TTL, 0), 0.0);
    texcoord_1  = vec2(texcoord_explosion(vertex[0].TTL, 1), 0.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    // Left-Top
    vec2 vb = P.xy + vec2(-0.5,  0.5) * particle_size;
    gl_Position = projection * vec4(vb, P.zw);
    texcoord_0  = vec2(texcoord_explosion(vertex[0].TTL, 0), 1.0);
    texcoord_1  = vec2(texcoord_explosion(vertex[0].TTL, 1), 1.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    // Right-Bottom
    vec2 vc = P.xy + vec2( 0.5, -0.5) * particle_size;
    gl_Position = projection * vec4(vc, P.zw);
    texcoord_0  = vec2(texcoord_explosion(vertex[0].TTL, 1), 0.0);
    texcoord_1  = vec2(texcoord_explosion(vertex[0].TTL, 2), 0.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    // Right-Top
    vec2 vd = P.xy + vec2( 0.5,  0.5) * particle_size;
    gl_Position = projection * vec4(vd, P.zw);
    texcoord_0  = vec2(texcoord_explosion(vertex[0].TTL, 1), 1.0);
    texcoord_1  = vec2(texcoord_explosion(vertex[0].TTL, 2), 1.0);
    TTL         = vertex[0].TTL;
    EmitVertex();

    EndPrimitive();
}
