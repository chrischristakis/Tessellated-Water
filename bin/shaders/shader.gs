#version 400 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float time;
uniform mat4 mvp;

in vec2 tes_uv[];
in vec3 tes_pos[];
out vec2 gs_uv;
out vec3 gs_normal;
out vec3 gs_vertpos;

uniform sampler2D dispMap;

vec3 Gerstner (vec3 worldpos, float w, float A, float phi, float Q, vec2 D, int N);
vec3 compute_normal(vec3 v0, vec3 v1, vec3 v2);

void main() {
	vec3 positions[3];  // Store Gerstner positions in world space

	// Loops through each vertex in our primitive (In our case, a triangle so 3)
	for(int i = 0; i < tes_pos.length(); i++) {
		vec3 position = tes_pos[i];
		float displacement = texture(dispMap, tes_uv[i]).r * 0.5; // greyscale so just use one color component, dimmed to 0.5
		position.y += displacement;

		// Combine multiple Gerstner waves using superposition and make a megawave
		position += Gerstner(tes_pos[i], 2.2, 0.09, 1.1, 0.83, vec2(0.3, 0.6), 4);
		position += Gerstner(tes_pos[i], 1.5, 0.15, 1.1, 0.75, vec2(0, -0.5), 4);
		position += Gerstner(tes_pos[i], 0.25, 0.19, 0.4, 0.4, vec2(0.3,  0.7), 4);
		position += Gerstner(tes_pos[i], 1.25, 0.11, 0.8, 0.6, vec2(-0.8,  0.1), 4);

		positions[i] = position;
	}

	// Use positions from Gerstner waves for normal
	vec3 normal = compute_normal(positions[0], positions[1], positions[2]);

	// Now that we have our normal, lets make some primitives
	for(int i = 0; i < positions.length(); i++) {
		// Go from world->clip here finally, now that we're done transforming.
		gl_Position = mvp * vec4(positions[i], 1.0);
		gs_uv = tes_uv[i];
		gs_normal = normal;
		gs_vertpos = positions[i];
		EmitVertex();
	}
	EndPrimitive();
}

// Ngl, no idea how the match actually works but lets treat it like a black box :)
// worldpos is original vertex pos, w is frequency of waves, A is amplitude, phi is phase shift
// Q is wave sharpness, D is drection of wave in 2 direction
vec3 Gerstner (vec3 worldpos, float w, float A, float phi, float Q, vec2 D, int N) {
	vec3 pos = vec3(0.0);

	Q /= w * A * N; // Helps us avoid the 'loops' caused in sharpness of a wave
	
	float dotprod = dot(w * D, worldpos.xz);
	pos.x = Q * A * D.x * cos(dotprod + phi*time);
	pos.y = A * sin(dotprod + phi*time);
	pos.z = Q * A * D.y * cos(dotprod + phi*time);

	return pos;
}

// Compute normals using 3 vertices, assuming ccw+
vec3 compute_normal(vec3 v0, vec3 v1, vec3 v2) {
	// calculate 2 vectors lying on plane of the triangle
	vec3 v02 = v2 - v0;
	vec3 v01 = v1 - v0;

	// Use cross product to get normal, order is important!
	return normalize(cross(v01, v02));
}