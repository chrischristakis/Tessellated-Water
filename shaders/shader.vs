#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec2 v_uv;
out vec3 v_pos;
out vec3 v_normal;

uniform float time;

void main() {
	v_uv = (uv - time*0.01)*2.0;
	v_normal = normal;
	v_pos = position;
}