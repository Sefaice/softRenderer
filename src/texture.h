#pragma once

#include "math.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

class Texture {
public:
	int texWidth;
	int texHeight;

	Texture(const char* path) {
		texData = stbi_load(path, &texWidth, &texHeight, &texNrChannels, 0);
		//printf("%d, %d %d %d\n", texWidth, texHeight, texNrChannels, sizeof(texData) / sizeof(unsigned char));
	}
	vec3 sampleTex(vec2 texCoords) {
		vec3 color;
		if (texCoords.x >= 0 && texCoords.x <= 1 && texCoords.y >= 0 && texCoords.y <= 1) {
			color = getColor(texCoords.x * (texWidth - 1), texCoords.y * (texHeight - 1));
		}

		return color;
	}
	vec3 sampleTex_bilinear(vec2 texCoords) {
		vec3 color;
		if (texCoords.x >= 0 && texCoords.x <= 1 && texCoords.y >= 0 && texCoords.y <= 1) {
			float texCoordsX = texCoords.x * texWidth;
			float texCoordsY = texCoords.y * texHeight;
			
			vec3 c1 = getColor(floor(texCoordsX), floor(texCoordsY));
			vec3 c2 = getColor(floor(texCoordsX) + 1, floor(texCoordsY));
			vec3 c3 = getColor(floor(texCoordsX), floor(texCoordsY) + 1);
			vec3 c4 = getColor(floor(texCoordsX) + 1, floor(texCoordsY) + 1);
			
			vec3 c12 = lerp(c1, c2, texCoordsX - floor(texCoordsX));
			vec3 c34 = lerp(c3, c4, texCoordsX - floor(texCoordsX));
			color = lerp(c12, c34, texCoordsY - floor(texCoordsY));
		}

		return color;
	}

private:
	int texNrChannels;
	unsigned char* texData;

	vec3 getColor(int texCoordsX, int texCoordsY) {
		vec3 color;
		color.x = texData[texCoordsY * 3 * texWidth + texCoordsX * 3] / 255.0;
		color.y = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 1] / 255.0;
		color.z = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 2] / 255.0;

		return color;
	}
};
