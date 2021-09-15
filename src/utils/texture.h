#pragma once

#include "math.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

class Texture {
public:
	std::string path;
	std::string type; // texture_diffuse, texture_specular, texture_normal, ...
	int texWidth;
	int texHeight;

	Texture(std::string _path, std::string _type, int _hdr) {
		path = _path;
		type = _type;
		hdr = _hdr;

		if (hdr == 0) {
			//texData = stbi_load(path.c_str(), &texWidth, &texHeight, &texNrChannels, 0);
			texData = stbi_load(path.c_str(), &texWidth, &texHeight, &texNrChannels, 3); // for "gun" model
			//std::cout << path << std::endl;
			//printf("%d, %d %d %d\n", texWidth, texHeight, texNrChannels, sizeof(texData) / sizeof(unsigned char));

			if (!texData) {
				std::cout << "Failed to load texture: " << path << std::endl;
				//throw std::invalid_argument("Failed to load texture");
			}
		}
		else {
			//stbi_set_flip_vertically_on_load(true);
			hdrTexData = stbi_loadf(path.c_str(), &texWidth, &texHeight, &texNrChannels, 3);
			/*std::cout << path << std::endl;
			printf("%d, %d %d %d\n", texWidth, texHeight, texNrChannels, sizeof(hdrTexData) / sizeof(float));*/

			if (!hdrTexData) {
				std::cout << "Failed to load HDR image." << std::endl;
			}
		}
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
	int hdr; // 1 for hdr texture
	float* hdrTexData;

	vec3 getColor(int texCoordsX, int texCoordsY) {
		vec3 color;

		if (hdr == 0) {
			color.x = texData[texCoordsY * 3 * texWidth + texCoordsX * 3] / 255.0;
			color.y = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 1] / 255.0;
			color.z = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 2] / 255.0;
		}
		else {
			color.x = hdrTexData[texCoordsY * 3 * texWidth + texCoordsX * 3];
			color.y = hdrTexData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 1];
			color.z = hdrTexData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 2];
		}

		return color;
	}
};

class CubeMapTexture {
public:
	std::vector<Texture*> faces; // +x, -x, +y, -y, +z, -z
	bool bilinear = false;

	CubeMapTexture(std::string path, bool _bilinear) {
		faces.push_back(new Texture(path + "/right.jpg", "texture_cubemap", 0));
		faces.push_back(new Texture(path + "/left.jpg", "texture_cubemap", 0));
		faces.push_back(new Texture(path + "/top.jpg", "texture_cubemap", 0));
		faces.push_back(new Texture(path + "/bottom.jpg", "texture_cubemap", 0));
		faces.push_back(new Texture(path + "/front.jpg", "texture_cubemap", 0));
		faces.push_back(new Texture(path + "/back.jpg", "texture_cubemap", 0));

		bilinear = _bilinear;
	}

	vec3 sampleCubeMap(vec3 texCoords) {
		float absx = abs(texCoords.x);
		float absy = abs(texCoords.y);
		float absz = abs(texCoords.z);

		float ma, uc, vc, u, v;
		unsigned int faceInd;
		if (absx > absy && absx > absz) {
			ma = absx;
			if (texCoords.x > 0) { // +x
				faceInd = 0;
				uc = -texCoords.z;
				vc = -texCoords.y;
			}
			else { // -x
				faceInd = 1;
				uc = texCoords.z;
				vc = -texCoords.y;
			}
		}
		else if (absy > absz) {
			ma = absy;
			if (texCoords.y > 0) { // +y
				faceInd = 2;
				uc = texCoords.x;
				vc = texCoords.z;
			}
			else { // -y
				faceInd = 3;
				uc = texCoords.x;
				vc = -texCoords.z;
			}
		}
		else {
			ma = absz;
			if (texCoords.z > 0) { // +z
				faceInd = 4;
				uc = texCoords.x;
				vc = -texCoords.y;
			}
			else { // -z
				faceInd = 5;
				uc = -texCoords.x;
				vc = -texCoords.y;
			}
		}

		u = 0.5f * (uc / ma + 1.0f);
		v = 0.5f * (vc / ma + 1.0f);
		//v = 0.5f * (-vc / ma + 1.0f);

		if (!bilinear)
			return faces[faceInd]->sampleTex(vec2(u, v));
		else
			return faces[faceInd]->sampleTex_bilinear(vec2(u, v));
	}
};