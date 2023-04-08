#include "Water.h"
#include "ImageData.h"

// Make a plane with an origin at [min, 0, min]
void planeMeshQuads(std::vector<Water::Vertex> &verts, std::vector<unsigned int> &indices, 
	float min, float max, float stepsize) {
	float x = min;
	float y = 0;

	for (float x = min; x <= max; x += stepsize) {
		for (float z = min; z <= max; z += stepsize) {

			// Linearly map [min, max] -> [0,1] for UV coordinates
			float u = (x - min) / (max - min);
			float v = (z - min) / (max - min);

			// Plane origin is (min, 0, min), we want the texture to face us properly, so slip v coord
			v = 1 - v;

			Water::Vertex vert;
			vert.position = glm::vec3(x, y, z);
			vert.uv = glm::vec2(u, v);
			vert.normal = glm::vec3(0, 1, 0);
			verts.emplace_back(vert);
		}
	}

	// Index data
	int colSize = (max - min) / stepsize + 1;
	int i = 0, j = 0;
	for (float x = min; x <= max - 1; x += stepsize) {
		j = 0;
		for (float z = min; z <= max - 1; z += stepsize) {
			
			// Patch indices
			indices.emplace_back(i * colSize + j);			// 0      3
			indices.emplace_back(i * colSize + j + 1);		//   ccw+
			indices.emplace_back((i + 1) * colSize + j + 1);//  
			indices.emplace_back((i + 1) * colSize + j);	// 1      2
			j++;
		}
		i++;
	}
}

Water::Water(float min, float max, float stepsize): VAO(0), VBO(0), EBO(0){
	// Generate a plane into our vert and index vectors
	planeMeshQuads(verts, indices, min, max, stepsize);

	// Now, generate our VAO and VBO data
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// UV
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::uv));
	glEnableVertexAttribArray(1);

	// Normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Now get texture data
	ImageData imgData("assets/water.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgData.width, imgData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData.data);
	glGenerateMipmap(GL_TEXTURE_2D);  // We need this for textures to work.
	imgData.free();

	// Displacement map texture
	ImageData dmData("assets/displacement-map1.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &dispMapTexture);
	glBindTexture(GL_TEXTURE_2D, dispMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dmData.width, dmData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, dmData.data);
	glGenerateMipmap(GL_TEXTURE_2D); 
	dmData.free();
}

void Water::draw(ShaderProgram *shader, glm::mat4 view, glm::mat4 projection) {
	glBindVertexArray(VAO);
	glUseProgram(shader->ID);

	shader->setUniformMatrix4fv("mvp", projection * view);
	shader->setUniform1i("tex", 0);
	shader->setUniform1i("dispMap", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dispMapTexture);

	glPatchParameteri(GL_PATCH_VERTICES, 4);  // We need to do this if we're using patches.
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}