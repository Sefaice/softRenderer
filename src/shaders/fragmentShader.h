#pragma once

#include "../utils/math.h"
#include "./utils/texture.h"

class FragmentShader {
public:
	vec3 lightColor;
	vec3 lightPos;
	vec3 viewPos;

	virtual vec3 shading(FS_in fin) {
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
	// FS_in:
	//     vec2 texCoords
	//     vec3 normal
	//     vec3 worldPos
public:
	vec3 shading(FS_in fin) {
		vec2 texCoords = fin.texCoords;
		vec3 normal = fin.normal;
		vec3 worldPos = fin.worldPos;

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
	// FS_in:
	//     vec2 texCoords
	//     vec3 normal
	//     vec3 worldPos
	//     mat3 TBN
public:
	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;

	vec3 shading(FS_in fin) {
		vec2 texCoords = fin.texCoords;
		vec3 normal = fin.normal;
		vec3 worldPos = fin.worldPos;
		mat3 TBN = fin.TBN;

		// TBN matrix, transform local normal to world space
		vec3 ln = normalMap->sampleTex(texCoords);
		vec3 n = TBN * ln;
		vec3 norm = normalize(n);

		//// use normal vec
		//vec3 norm = normalize(normal);

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
	// FS_in:
	//     vec3 cubeMapTexCoords
public:

	// uniform
	CubeMapTexture* cubeMapTexture;

	vec3 shading(FS_in fin) {
		vec3 cubeMapTexCoords = fin.cubeMapTexCoords;

		vec3 color = cubeMapTexture->sampleCubeMap(cubeMapTexCoords);

		return color;
	}
};

class EMapFragmentShader : public FragmentShader {
	// FS_in:
	//     vec3 normal
	//     vec3 worldPos
public:

	// uniform
	CubeMapTexture* cubeMapTexture;

	vec3 shading(FS_in fin) {
		vec3 normal = fin.normal;
		vec3 worldPos = fin.worldPos;

		// reflect
		vec3 I = normalize(worldPos - viewPos);
		vec3 reflectDir = reflect(I, normalize(normal));

		// refract
		float ratio = 1.00 / 1.52; // glass
		vec3 refractDir = refract(I, normalize(normal), ratio);

		vec3 color = cubeMapTexture->sampleCubeMap(refractDir);

		return color;
	}

};

class DepthFragmentShader : public FragmentShader {
public:
	vec3 shading(FS_in fin) {
		// do nothing
		return vec3();
	}
};

class ShadowFragmentShader : public FragmentShader {
public:

	// uniforms
	double* depthMap;
	int depthMapHeight;
	int depthMapWidth;

	vec3 shading(FS_in fin) {
		vec2 texCoords = fin.texCoords;
		vec3 normal = fin.normal;
		vec3 worldPos = fin.worldPos;
		vec4 posLightSpace = fin.posLightSpace;

		vec3 texColor = vec3(0.5f, 0.6f, 0.7f);
		vec3 lightDir = lightPos - worldPos;
		lightDir = normalize(lightDir);
		vec3 norm = normalize(normal);

		//float shadow = calcShadow(posLightSpace, norm, lightDir);
		//std::cout << shadow << std::endl;

		return texColor;
		//return vec3(shadow);
	}

private:

	float sampleDepthMap(vec2 texCoords) {

		int x = texCoords.x * (depthMapWidth - 1);
		int y = texCoords.y * (depthMapHeight - 1);
		
		double* tmp = depthMap + y * depthMapWidth + x;

		return tmp[0];
	}

	// in light space
	float calcShadow(vec4 posLightSpace, vec3 norm, vec3 lightDir)
	{
		float shadow = 0;

		vec3 ndcCoords = vector3(posLightSpace) / posLightSpace.w;
		ndcCoords = (ndcCoords + 1.0) / 2;

		float closestDepth = sampleDepthMap(vec2(ndcCoords.x, ndcCoords.y));

		// DEBUG
		// -------------------------------------------
		//			if(closestDepth > 0.0 && closestDepth < 1.0)
		//				FragColor = vec4(debugColor * 3, 1.0);
		//			else 
		//				FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		//			FragColor = vec4(vec3(closestDepth), 1.0);
		// --------------------------------------

		float t_frustum_n = .1f;
		float t_frustum_f = 100.0f;
		float currentDepth = ndcCoords.z;
		float currentDepthz = (t_frustum_f - t_frustum_n) / 2 * currentDepth + (t_frustum_f + t_frustum_n) / 2;
		float t_frustum_nf1 = t_frustum_f / (t_frustum_f - t_frustum_n);
		float t_frustum_nf2 = t_frustum_f * t_frustum_n / (t_frustum_n - t_frustum_f);
		currentDepthz = t_frustum_nf1 + t_frustum_nf2 / currentDepthz;

		if (currentDepth > 1.0) {
			shadow = 0;
		}
		else
		{
			float bias = maxInTwo(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
			//shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
			shadow = currentDepth > closestDepth ? 1.0 : 0.0;
			/*vec2 texelSize = 1.0 / textureSize(depthMap[index], 0);
			for (int x = -1; x <= 1; ++x)
			{
				for (int y = -1; y <= 1; ++y)
				{
					float pcfDepth = texture(depthMap[index], ndcCoords.xy + vec2(x, y) * texelSize).r;
					shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
				}
			}
			shadow /= 9.0;*/
		}

		return currentDepthz;
	}
};