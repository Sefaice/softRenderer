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
		// blinn-phong
		vec3 halfwayDir = lightDir + viewDir;
		halfwayDir = normalize(halfwayDir);
		float spec = pow(maxInTwo(dot(norm, halfwayDir), 0.0), 32);
		vec3 specular = lightColor * specularStrength * spec;

		float shadow = calcShadow(posLightSpace, norm, lightDir);

		vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;

		//color = vec3(shadow);
		//color = texColor;

		return color;
	}

private:

	double sampleDepthMap(vec2 texCoords) {

		int x = texCoords.x * (depthMapWidth - 1);
		int y = texCoords.y * (depthMapHeight - 1);
		
		double* tmp = depthMap + y * depthMapWidth + x;

		return tmp[0];
	}

	// in light space
	float calcShadow(vec4 posLightSpace, vec3 norm, vec3 lightDir)
	{
		double shadow = 0;

		vec3 ndcCoords = vector3(posLightSpace) / posLightSpace.w;
		ndcCoords = (ndcCoords + 1.0) / 2.0;

		double currentDepth = ndcCoords.z;

		double closestDepth = sampleDepthMap(vec2(ndcCoords.x, ndcCoords.y));

		if (currentDepth > 1.0) {
			shadow = 0;
		}
		else
		{
			float bias = maxInTwo(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
			//shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

			// pcf
			for (int x = -3; x <= 3; ++x)
			{
				for (int y = -3; y <= 3; ++y)
				{
					double pcfDepth = sampleDepthMap(vec2(ndcCoords.x + (double)x / depthMapWidth, ndcCoords.y + (double)y / depthMapHeight));
					shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
				}
			}
			shadow /= 49.0;
		}

		return shadow;
	}
};

class PBRFragmentShader : public FragmentShader {
	// FS_in:
	//     vec2 texCoords
	//     vec3 normal
	//     vec3 worldPos
public:
	Texture* albedoMap;
	Texture* normalMap;
	Texture* metallicMap;
	Texture* roughnessMap;
	Texture* aoMap;

	vec3 shading(FS_in fin) {
		vec2 texCoords = fin.texCoords;
		vec3 normal = fin.normal;
		vec3 worldPos = fin.worldPos;

		vec3 albedo = albedoMap->sampleTex(texCoords);
		// sRGB -> linear space
		albedo = pow(albedo, vec3(2.2));
		vec3 norm = normalize(normal);
		float metallic = metallicMap->sampleTex(texCoords).x;
		float roughness = roughnessMap->sampleTex(texCoords).x;
		float ao = aoMap->sampleTex(texCoords).x;

		vec3 viewDir = viewPos - worldPos;
		viewDir = normalize(viewDir);
		vec3 lightDir = lightPos - worldPos;
		lightDir = normalize(lightDir);

		// radiance
		vec3 h = normalize(viewDir + lightDir);
		float distance = (lightPos - worldPos).length();
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColor * attenuation;

		// BRDF
		float D = normalDistribution(norm, h, roughness);
		float G = geometrySub(norm, viewDir, roughness) * geometrySub(norm, lightDir, roughness); //  Smith's method
		vec3 F0 = lerp(vec3(0.04), albedo, metallic);
		vec3 F = fresnelEquation(h, viewDir, F0);

		float nvDot = maxInTwo(dot(norm, viewDir), 0);
		float nlDot = maxInTwo(dot(norm, lightDir), 0);
		float denom = 4 * nvDot * nlDot;
		float specular = D * G / maxInTwo(denom, 0.001);

		vec3 ks = F;
		vec3 kd = vec3(1.0) - ks;
		kd = kd * vec3(1.0 - metallic); // metallic do not have diffuse

		vec3 l = (kd * albedo / M_PI + ks * specular) * radiance * nlDot;

		vec3 ambient = vec3(0.03) * albedo * ao;

		vec3 color = ambient + l;

		// HDR tonemapping
		color = color / (color + vec3(1.0));
		// gamma correct
		color = pow(color, vec3(1.0 / 2.2));

		return color;
	}

private:

	// Trowbridge-Reitz GGX
	float normalDistribution(vec3 n, vec3 h, float a) {
		float a4 = pow(a, 4.0);
		float nhDot = maxInTwo(dot(n, h), 0);
		float denom = M_PI * pow(nhDot * nhDot * (a4 - 1.0) + 1.0, 2.0);

		return a4 / maxInTwo(denom, 0.001); // prevent division by 0
	}

	// Schlick-GGX
	float geometrySub(vec3 n, vec3 v, float a) {
		float k = pow(a + 1.0, 2.0) / 8.0;
		float nvDot = maxInTwo(dot(n, v), 0);

		return nvDot / (nvDot * (1.0 - k) + k);
	}

	// Fresnel Equation
	vec3 fresnelEquation(vec3 h, vec3 v, vec3 F0) {
		float hvDot = dot(h, v);
		return F0 + (1 - F0) * pow(1 - hvDot, 5.0);
	}
};
