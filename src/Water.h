#ifndef WATER_H
#define WATER_H
#include <glm/mat4x4.hpp>
#include <vector>
#include "ShaderProgram.h"

class Water {
private:
	struct Vertex;

	GLuint texture, dispMapTexture, VAO, VBO, EBO;
	std::vector<Vertex> verts;
	std::vector<unsigned int> indices;
public:
	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	Water(float min, float max, float stepsize);
	void draw(ShaderProgram* shader, glm::mat4 view, glm::mat4 projection);
};

#endif