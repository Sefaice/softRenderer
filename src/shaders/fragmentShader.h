#pragma once

#include "../utils/math.h"
#include ".//utils/texture.h"

class FragmentShader {
public:
	vec3 lightColor;
	vec3 lightPos;
	vec3 viewPos;

	virtual vec3 shading(Vertex v) {
		std::cout << "error! virtual f shading" << std::endl;
		return vec3();
	}

	//vec3 shading_texture(vec2 texCoords) {
	//	vec3 texColor = diffuseMap->sampleTex(texCoords);
	//	//vec3 texColor = texture->sampleTex_bilinear(texCoords);

	//	return texColor;
	//}

	vec3 shading_bump(Texture* texture, vec3 normal, vec2 texCoords, vec3 worldPos) {
		float kh = 3.2, kn = 1.6;

		// local normal
		float dU = kh * kn * (h(texture, texCoords.x + 1.0f / texture->texWidth, texCoords.y) - h(texture, texCoords.x, texCoords.y));
		float dV = kh * kn * (h(texture, texCoords.x, texCoords.y + 1.0f / texture->texHeight) - h(texture, texCoords.x, texCoords.y));
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

	vec3 shading_displacement(Texture* texture, vec3 normal, vec2 texCoords, vec3 worldPos) {
		float kh = 3.2, kn = 1.6;

		// local normal
		float dU = kh * kn * (h(texture, texCoords.x + 1.0f / texture->texWidth, texCoords.y) - h(texture, texCoords.x, texCoords.y));
		float dV = kh * kn * (h(texture, texCoords.x, texCoords.y + 1.0f / texture->texHeight) - h(texture, texCoords.x, texCoords.y));
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
		vec3 worldPos_dis = worldPos + kn * n * h(texture, texCoords.x, texCoords.y);

		return worldPos_dis;
	}

protected:
	
	float h(Texture* texture, float u, float v) { // GAMES101这么计算，没有意义，只是一个随机选择的得到灰度图的方法
		vec3 texColor = texture->sampleTex(vec2(u, v));
		return sqrt(texColor.x * texColor.x + texColor.y * texColor.y + texColor.z * texColor.z);
	}
};

class PhongFragmentShader : public FragmentShader {
public:

	vec3 shading(Vertex v) {
		vec3 normal = v.normal;
		vec2 texCoords = v.texCoords;
		vec3 worldPos = v.worldPos;

		vec3 texColor = vec3(0.5f, 0.6f, 0.7f);

		// ambient
		float ambientStrength = 0.2;
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
		float spec = pow(maxInTwo(dot(norm, halfwayDir), 0.0), 32);
		vec3 specular = lightColor * specularStrength * spec;

		vec3 color = (ambient + diffuse + specular) * texColor;

		return color;
	}

};

class ObjFragmentShader : public FragmentShader {
public:
	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;

	vec3 shading(Vertex v) {
		vec3 normal = v.normal;
		vec2 texCoords = v.texCoords;
		vec3 worldPos = v.worldPos; 
		
		//mat3 TBN = v.TBN;
		//// TBN matrix, transform local normal to world space
		//vec3 ln = normalMap->sampleTex(texCoords);
		//vec3 n = TBN * ln;
		//vec3 norm = normalize(n);

		// use normal vec
		vec3 norm = normalize(normal);

		vec3 diffuseColor = diffuseMap->sampleTex(texCoords);

		// ambient
		vec3 ambient = lightColor * diffuseColor;

		// diffuse
		vec3 lightDir = lightPos - worldPos;
		lightDir = normalize(lightDir);
		float diff = maxInTwo(dot(norm, lightDir), 0.0);
		vec3 diffuse = lightColor * diff * diffuseColor;

		// specular
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
		vec3 specular = lightColor * spec * specularMap->sampleTex(texCoords);

		vec3 color = ambient + diffuse + specular;

		return color;
	}
};

class CubeMapFragmentShader : public FragmentShader {
public:
	CubeMapTexture* cubeMapTexture;

	vec3 shading(Vertex v) {
		vec3 color = cubeMapTexture->sampleCubeMap(v.cubeMapTexCoords);

		return color;
	}
};