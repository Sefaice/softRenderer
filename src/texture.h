#pragma once

#include "math.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#include <opencv2/opencv.hpp>

class Texture {
public:
	int texWidth;
	int texHeight;

	Texture(const char* path) {
		texData = stbi_load(path, &texWidth, &texHeight, &texNrChannels, 0);
		//printf("%d, %d %d %d\n", texWidth, texHeight, texNrChannels, sizeof(texData) / sizeof(unsigned char));

		image_data = cv::imread(path);
		cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
		texWidth = image_data.cols;
		texHeight = image_data.rows;
		//printf("%d, %d\n", texWidth, texHeight);

	}
	vec3 sampleTex(vec2 texCoords) {
		vec3 color;
		//if (texCoords.x >= 0 && texCoords.x <= 1 && texCoords.y >= 0 && texCoords.y <= 1) {
		//	int texCoordsX = texCoords.x * texWidth;
		//	int texCoordsY = texCoords.y * texHeight;
		//	//t1.print(); t2.print(); t3.print();
		//	//printf("%f %f %d %d\n", texCoords.x, texCoords.y, texCoordsX, texCoordsY);
		//	color.x = texData[texCoordsY * 3 * texWidth + texCoordsX * 3] / 255.0;
		//	color.y = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 1] / 255.0;
		//	color.z = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 2] / 255.0;
		//}
		//else {
		//	//texCoords.print();
		//}

		if (texCoords.x < 0) texCoords.x += 1.0;
		if (texCoords.x > 1) texCoords.x -= 1.0;
		if (texCoords.y < 0) texCoords.y += 1.0;
		if (texCoords.y > 1) texCoords.y -= 1.0;



		int texCoordsX = texCoords.x * texWidth;
		int texCoordsY = texCoords.y * texHeight;
		//t1.print(); t2.print(); t3.print();
		//printf("%f %f %d %d\n", texCoords.x, texCoords.y, texCoordsX, texCoordsY);
		color.x = texData[texCoordsY * 3 * texWidth + texCoordsX * 3] / 255.0;
		color.y = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 1] / 255.0;
		color.z = texData[texCoordsY * 3 * texWidth + texCoordsX * 3 + 2] / 255.0;


		return color;
	}
	vec3 getColor(vec2 texCoords) {
		vec3 color;
		if (texCoords.x >= 0 && texCoords.x <= 1 && texCoords.y >= 0 && texCoords.y <= 1) {
			auto u_img = texCoords.x * texWidth;
			auto v_img = (1 - texCoords.y) * texHeight;
			auto c = image_data.at<cv::Vec3b>(v_img, u_img);
			color = vec3(c[0] / 255.0, c[1] / 255.0, c[2] / 255.0);
			color.print();
		}
		
		return color;
	}

private:
	int texNrChannels;
	unsigned char* texData;

	cv::Mat image_data;
};
