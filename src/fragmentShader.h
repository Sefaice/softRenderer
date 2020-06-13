#pragma once

#include "math.h"
#include "texture.h"

class FragmentShader {
public:
	FragmentShader(Texture* _texture, vec3 _lightColor, vec3 _lightPos, vec3 _viewPos)
		: texture(_texture), lightColor(_lightColor), lightPos(_lightPos), viewPos(_viewPos) {}
	vec3 shading_texture(vec2 texCoords) {
		vec3 texColor = texture->sampleTex(texCoords);
		//vec3 texColor = texture->getColor(texCoords);

		/*if (texCoords.x >= 0.1 && texCoords.x <= 1.0 && texCoords.y >= 0.1 && texCoords.y <= 1.0) {
			return vec3(0, 0, 1);
		}*/
		/*if (texCoords.y >= 0.0 && texCoords.y <= 0.1) return vec3(0, 0, 0.1);
		if (texCoords.y >= 0.1 && texCoords.y <= 0.2) return vec3(0, 0, 0.2);
		if (texCoords.y >= 0.2 && texCoords.y <= 0.3) return vec3(0, 0, 0.3);
		if (texCoords.y >= 0.3 && texCoords.y <= 0.4) return vec3(0, 0, 0.4);
		if (texCoords.y >= 0.4 && texCoords.y <= 0.5) return vec3(0, 0, 0.5);
		if (texCoords.y >= 0.5 && texCoords.y <= 0.6) return vec3(0, 0, 0.6);
		if (texCoords.y >= 0.6 && texCoords.y <= 0.7) return vec3(0, 0, 0.7);
		if (texCoords.y >= 0.7 && texCoords.y <= 0.8) return vec3(0, 0, 0.8);
		if (texCoords.y >= 0.8 && texCoords.y <= 0.9) return vec3(0, 0, 0.9);
		if (texCoords.y >= 0.9 && texCoords.y <= 1.0) return vec3(0, 0, 1);*/


		return texColor;
		//return vec3(texCoords.x, texCoords.y, 0);
	}
	vec3 shading_phong(vec3 normal, vec2 texCoords, vec3 worldPos) {
		// sample in texture map
		vec3 texColor = texture->sampleTex(texCoords);
		//vec3 texColor = vec3(1.0, 0.5, 0.31);

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

		float kh = 0.2, kn = 0.1;

		// TODO: Implement bump mapping here
		// Let n = normal = (x, y, z)
		// Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
		// Vector b = n cross product t
		// Matrix TBN = [t b n]
		// dU = kh * kn * (h(u+1/w,v)-h(u,v))
		// dV = kh * kn * (h(u,v+1/h)-h(u,v))
		// Vector ln = (-dU, -dV, 1)
		// Normal n = normalize(TBN * ln)

		float x2z2 = sqrt(normal.x * normal.x + normal.z * normal.z);
		vec3 t = vec3(normal.x * normal.y / x2z2, x2z2, normal.z * normal.y / x2z2);
		vec3 b = cross(normal, t);
		mat3 TBN = mat3(t.x, t.y, t.z, b.x, b.y, b.z, normal.x, normal.y, normal.z);

		float dU = kh * kn * (h(texCoords.x + 1.0f / texture->texWidth, texCoords.y) - h(texCoords.x, texCoords.y));
		float dV = kh * kn * (h(texCoords.x, texCoords.y + 1.0f / texture->texHeight) - h(texCoords.x, texCoords.y));
		vec3 ln = vec3(-dU, -dV, 1);

		vec3 n = TBN * ln;
		n = normalize(n);

		normal.print();
		n.print();
		printf(".......................\n");

		return n;
	}

private:
	Texture* texture;
	vec3 lightColor;
	vec3 lightPos;
	vec3 viewPos;

	float h(int u, int v) { // GAMES101 ��ô����û�����壬Ϊ���ѡ��ĵõ��Ҷ�ͼ�ķ���
		vec3 texColor = texture->sampleTex(vec2(u, v));
		return sqrt(texColor.x * texColor.x + texColor.y * texColor.y + texColor.z * texColor.z);
	}
};