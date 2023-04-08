#version 400 core

out vec4 fragCol;

in vec2 gs_uv;
in vec3 gs_normal;
in vec3 gs_vertpos;

uniform sampler2D tex;
uniform vec3 eyeDir;
uniform vec3 lightDir;

void main() {
	vec3 normLightDir = normalize(-lightDir);
	vec3 normal = normalize(gs_normal);

	vec3 ambient = vec3(0.3, 0.5, 0.5) * texture(tex, gs_uv).rgb;

	vec3 diffuseColor = texture(tex, gs_uv).rgb;
	float diffuseCoefficient = max(dot(normal, normLightDir), 0);
	vec3 diffuse = diffuseColor * diffuseCoefficient;

	//Since everything is in view space, we can just use the vector from 0,0,0 to the vert pos as eye direction.
	vec3 specularColor = vec3(0.9, 0.95, 0.99);
	vec3 reflection = reflect(-normLightDir, normal);
	vec3 eyedir = normalize(eyeDir - gs_vertpos);
	float specularCoefficient = pow(max(dot(eyedir, reflection), 0), 8);
	vec3 specular = specularColor * specularCoefficient;

	fragCol = vec4(ambient + diffuse + specular, 1.0);
}