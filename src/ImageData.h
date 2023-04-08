#ifndef IMAGEDATA_H
#define IMAGEDATA_H
#define STB_IMAGE_IMPLEMENTATION // STB needs this definition once
#include <stb/stb_image.h>
#include <iostream>

struct ImageData {
	int width, height, nChannels;
	unsigned char* data;
	bool loaded;

	ImageData(const char* path): loaded(false) {
		stbi_set_flip_vertically_on_load(true); // STB loads images upside down
		data = stbi_load(path, &width, &height, &nChannels, 0);
		if (data == NULL) {
			std::cout << "Failed to load " << path << ": " << stbi_failure_reason() << std::endl;
			return;
		}
		loaded = true;
	}

	void free() {
		stbi_image_free(data);
	}
};

#endif