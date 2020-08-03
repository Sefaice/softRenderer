#pragma once

#include "math.h"
#include "texture.h"

class FragmentShader {
public:
	FragmentShader(Texture* _texture, vec3 _lightColor, vec3 _lightPos, vec3 _viewPos)
		: texture(_texture), lightColor(_lightColor), lightPos(_lightPos), viewPos(_viewPos) {}
	vec3 shading_texture(vec2 texCoords) {
		vec3 texColor = texture->sampleTex(texCoords);
		//vec3 texColor = texture->sampleTex_bilinear(texCoords);

		return texColor;
	}
	vec3 shading_phong(vec3 normal, vec2 texCoords, vec3 worldPos) {
		// sample in texture map
		//vec3 texColor = texture->sampleTex(texCoords);
		vec3 texColor = vec3(1.0, 0.5, 0.31);

		// ambient
		float ambientStrength = 0.5;
		vec3 ambient = lightColor * ambientStrength;

		// diffuse
		vec3 norm = normalize(normal);
		vec3 lightDir = lightPos - worldPos;
		lightDir = normalize(lightDir);
		float diff = maxInTwo(dot(norm, lightDir), 0.0);
		vec3 diffuse = lightColor * diff;

		// specular
		float specularStrength = 0.5;
		vec3 viewDir = viewPos - worldPos;
		viewDir = normalize(viewDir);
		//// phong
		//vec3 reflectDir = reflect(-lightDir, norm);
		//reflectDir = normalize(reflectDir);
		//float spec = pow(maxInTwo(dot(viewDir, reflectDir), 0.0), 16);
		// blinn-phong
		vec3 halfwayDir = lightDir + viewDir;
		halfwayDir = normalize(halfwayDir);
		float spec = pow(maxInTwo(dot(norm, halfwayDir), 0.0), 64);
		vec3 specular = lightColor * specularStrength * spec;

		vec3 color = (ambient + diffuse + specular) * texColor;

		return color;
	}
	vec3 shading_bump(vec3 normal, vec2 texCoords, vec3 worldPos) {
		float kh = 3.2, kn = 1.6;

		// local normal
		float dU = kh * kn * (h(texCoords.x + 1.0f / texture->texWidth, texCoords.y) - h(texCoords.x, texCoords.y));
		float dV = kh * kn * (h(texCoords.x, texCoords.y + 1.0f / texture->texHeight) - h(texCoords.x, texCoords.y));
		vec3 ln = vec3(-dU, -dV, 1);
		ln = normalize(ln);

		// TBN matrix, transform local normal to world space
		float x2z2 = sqrt(normal.x * normal.x + normal.z * normal.z);
		vec3 t = vec3(normal.x * normal.y / x2z2, -x2z2, normal.z * normal.y / x2z2);
		vec3 b = cross(normal, t);
		mat3 TBN = mat3(t.x, t.y, t.z, b.x, b.y, b.z, normal.x, normal.y, normal.z);
		vec3 n = TBN * ln;
		n = normalize(n);

		return n;
	}
	vec3 shading_displacement(vec3 normal, vec2 texCoords, vec3 worldPos) {
		float kh = 3.2, kn = 1.6;

		// local normal
		float dU = kh * kn * (h(texCoords.x + 1.0f / texture->texWidth, texCoords.y) - h(texCoords.x, texCoords.y));
		float dV = kh * kn * (h(texCoords.x, texCoords.y + 1.0f / texture->texHeight) - h(texCoords.x, texCoords.y));
		vec3 ln = vec3(-dU, -dV, 1);
		ln = normalize(ln);

		// TBN matrix, transform local normal to world space
		float x2z2 = sqrt(normal.x * normal.x + normal.z * normal.z);
		vec3 t = vec3(normal.x * normal.y / x2z2, -x2z2, normal.z * normal.y / x2z2);
		vec3 b = cross(normal, t);
		mat3 TBN = mat3(t.x, t.y, t.z, b.x, b.y, b.z, normal.x, normal.y, normal.z);
		vec3 n = TBN * ln;
		n = normalize(n);

		// displaced position
		vec3 worldPos_dis = worldPos + kn * n * h(texCoords.x, texCoords.y);

		return worldPos_dis;
	}

private:
	Texture* texture;
	vec3 lightColor;
	vec3 lightPos;
	vec3 viewPos;

	float h(float u, float v) { // GAMES101这么计算，没有意义，只是一个随机选择的得到灰度图的方法
		vec3 texColor = texture->sampleTex(vec2(u, v));
		return sqrt(texColor.x * texColor.x + texColor.y * texColor.y + texColor.z * texColor.z);
	}
};
