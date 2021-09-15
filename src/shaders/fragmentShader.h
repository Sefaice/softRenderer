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
		//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
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
		//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
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

		//// HDR tonemap and gamma correct
		//color = color / (color + vec3(1.0));
		//color = pow(color, vec3(1.0 / 2.2));

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

		// reinhard HDR tonemapping
		color = color / (color + vec3(1.0));
		// gamma correct
		color = pow(color, vec3(1.0 / 2.2));

		return color;
	}

protected:

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
		float hvDot = maxInTwo(dot(h, v), 0);
		return F0 + (1 - F0) * pow(1 - hvDot, 5.0);
	}
};

// equirectangularMap to cubemap
class E2CFragmentShader : public FragmentShader {
	// FS_in:
	//     vec3 worldPos
public:
	Texture* equirectangularMap;
	const vec2 invAtan = vec2(0.1591, 0.3183);

	vec3 shading(FS_in fin) {
		vec3 worldPos = fin.worldPos;

		vec2 uv = SampleSphericalMap(normalize(worldPos)); // make sure to normalize localPos
		vec3 texColor = equirectangularMap->sampleTex(uv);

		vec3 color = texColor;

		// reinhard HDR tonemapping
		color = color / (color + vec3(1.0));
		// gamma correct
		color = pow(color, vec3(1.0 / 2.2));

		return color;
	}

private:
	vec2 SampleSphericalMap(vec3 v)
	{
		vec2 uv = vec2(atan2(v.z, v.x), asin(v.y));
		uv = uv * invAtan;
		uv = uv + 0.5;
		return uv;
	}
};

// pre-compute irradiance
class IrradianceFragmentShader : public FragmentShader {
	// FS_in:
	//     vec3 worldPos
public:
	CubeMapTexture* cubeMapTexture;

	vec3 shading(FS_in fin) {
		vec3 worldPos = fin.worldPos;

		vec3 normal = normalize(worldPos);
		vec3 up = vec3(0, 1, 0);
		/*vec3 right = normalize(cross(up, normal));
		up = normalize(cross(normal, right));*/
		vec3 right = (cross(up, normal));
		up = (cross(normal, right));

		vec3 irradiance = vec3(0.0);

		float sampleDelta = 0.025;
		int nrSamples = 0;
		for (float phi = 0.0; phi < 2.0 * M_PI; phi += sampleDelta)
		{
			for (float theta = 0.0; theta < 0.5 * M_PI; theta += sampleDelta)
			{
				vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
				vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

				irradiance = irradiance + cubeMapTexture->sampleCubeMap(sampleVec) * cos(theta) * sin(theta);
				nrSamples++;
			}
		}

		irradiance = M_PI * irradiance / float(nrSamples);

		vec3 color = irradiance;

		return color;
	}
};

// pre-filter environmentMap
class PreFilterFragmentShader : public FragmentShader {
	// FS_in:
	//     vec3 worldPos
public:
	CubeMapTexture* cubeMapTexture;

	vec3 shading(FS_in fin) {
		vec3 worldPos = fin.worldPos;

		vec3 normal = normalize(worldPos);
		// simplyfying assumption that viewDir equals reflectDir equals the normal 
		vec3 R = normal;
		vec3 V = R;

		const unsigned int SAMPLE_COUNT = 1024;
		vec3 prefilteredColor = vec3(0.0);
		float totalWeight = 0.0;

		float roughness = 0.1; // corresponding to mipmap lavel, 0~1

		for (int i = 0; i < SAMPLE_COUNT; i++) {

			// importance sampling
			vec2 Xi = hammersley(i, SAMPLE_COUNT);
			vec3 H = importanceSampleGGX(Xi, normal, roughness);
			vec3 L = normalize(2.0 * dot(V, H) * H - V);

			float NdotL = maxInTwo(dot(normal, L), 0.0);
			if (NdotL > 0.0)
			{
				// sample from the environment's mip level based on roughness/pdf
				float D = normalDistribution(normal, H, roughness);
				float NdotH = maxInTwo(dot(normal, H), 0.0);
				float HdotV = maxInTwo(dot(H, V), 0.0);
				float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

				float resolution = 1024.0; // resolution of source cubemap (per face)
				float saTexel = 4.0 * M_PI / (6.0 * resolution * resolution);
				float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

				/*float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
				prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;*/
				prefilteredColor = prefilteredColor + cubeMapTexture->sampleCubeMap(L) * NdotL;
				
				totalWeight += NdotL;
			}
		}

		prefilteredColor = prefilteredColor / totalWeight;
		
		vec3 color = prefilteredColor;

		return color;
	}

protected:

	float radicalInverse_VdC(unsigned int bits) {
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	vec2 hammersley(unsigned int i, unsigned int N) {
		return vec2(float(i) / float(N), radicalInverse_VdC(i));
	}

	vec3 importanceSampleGGX(vec2 Xi, vec3 N, float a)
	{
		a = a * a;

		float phi = 2.0 * M_PI * Xi.x;
		float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
		float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

		vec3 H;
		H.x = cos(phi) * sinTheta;
		H.y = sin(phi) * sinTheta;
		H.z = cosTheta;

		vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
		vec3 tangent = normalize(cross(up, N));
		vec3 bitangent = cross(N, tangent);

		vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;

		return normalize(sampleVec);
	}

	// Trowbridge-Reitz GGX
	float normalDistribution(vec3 n, vec3 h, float a) {
		float a4 = pow(a, 4.0);
		float nhDot = maxInTwo(dot(n, h), 0);
		float denom = M_PI * pow(nhDot * nhDot * (a4 - 1.0) + 1.0, 2.0);

		return a4 / maxInTwo(denom, 0.001); // prevent division by 0
	}
};

class BRDFInteFragmentShader : public PreFilterFragmentShader {
public:
	vec3 shading(FS_in fin) {
		vec2 texCoords = fin.texCoords;

		vec2 inte = integrateBRDF(texCoords.x, texCoords.y);
		vec3 color = vec3(inte.x, inte.y, 0);

		return color;
	}

private:
	vec2 integrateBRDF(float NdotV, float a) {
		vec3 V;
		V.x = sqrt(1.0 - NdotV * NdotV);
		V.y = 0.0;
		V.z = NdotV;

		float A = 0.0;
		float B = 0.0;

		vec3 N = vec3(0.0, 0.0, 1.0);

		const unsigned int SAMPLE_COUNT = 1024;
		for (int i = 0; i < SAMPLE_COUNT; ++i)
		{
			// generates a sample vector that's biased towards the
			// preferred alignment direction (importance sampling).
			vec2 Xi = hammersley(i, SAMPLE_COUNT);
			vec3 H = importanceSampleGGX(Xi, N, a);
			vec3 L = normalize(2.0 * dot(V, H) * H - V);

			float NdotL = maxInTwo(L.z, 0.0);
			float NdotH = maxInTwo(H.z, 0.0);
			float VdotH = maxInTwo(dot(V, H), 0.0);

			if (NdotL > 0.0)
			{
				float G = geometrySubIBL(N, V, a) * geometrySubIBL(N, L, a);
				float G_Vis = (G * VdotH) / (NdotH * NdotV);
				float Fc = pow(1.0 - VdotH, 5.0);

				A += (1.0 - Fc) * G_Vis;
				B += Fc * G_Vis;
			}
		}

		A /= float(SAMPLE_COUNT);
		B /= float(SAMPLE_COUNT);

		return vec2(A, B);
	}

	float geometrySubIBL(vec3 n, vec3 v, float a)
	{
		float k = (a * a) / 2.0; // different from direct lighting
		float nvDot = maxInTwo(dot(n, v), 0);

		return nvDot / nvDot * (1.0 - k) + k;
	}
};

class IBLFragmentShader : public PBRFragmentShader {
	// FS_in:
	//     vec2 texCoords
	//     vec3 normal
	//     vec3 worldPos
public:
	CubeMapTexture* irradianceMap;
	CubeMapTexture* prefilterMap;
	Texture*        brdfLUT;

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

		/*metallic = 0.5;
		roughness = 0.5;
		albedo = vec3(0.5, 0, 0);
		ao = 1.0;*/
		vec3 F0 = lerp(vec3(0.04), albedo, metallic);

		vec3 viewDir = viewPos - worldPos;
		viewDir = normalize(viewDir);
		vec3 lightDir = lightPos - worldPos;
		lightDir = normalize(lightDir);
		vec3 reflectDir = reflect(-viewDir, normal);
		reflectDir = normalize(reflectDir);

		/********* direct light *********/

		// radiance
		vec3 h = normalize(viewDir + lightDir);
		float distance = (lightPos - worldPos).length();
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColor * attenuation;

		// BRDF
		float D = normalDistribution(norm, h, roughness);
		float G = geometrySub(norm, viewDir, roughness) * geometrySub(norm, lightDir, roughness); //  Smith's method
		
		vec3 F = fresnelEquation(h, viewDir, F0);

		float nvDot = maxInTwo(dot(norm, viewDir), 0);
		float nlDot = maxInTwo(dot(norm, lightDir), 0);
		float denom = 4 * nvDot * nlDot;
		float specular = D * G / maxInTwo(denom, 0.001);

		vec3 ks = F;
		vec3 kd = vec3(1.0) - ks;
		kd = kd * vec3(1.0 - metallic); // metallic do not have diffuse

		vec3 l = (kd * albedo / M_PI + ks * specular) * radiance * nlDot;

		/**************************************************/

		/********* ambient indirect light *********/

		vec3 am_ks = fresnelEquationRoughness(norm, viewDir, F0, roughness);

		vec3 am_kd = 1.0 - am_ks;
		am_kd = am_kd * (1.0 - metallic);

		// diffuse
		vec3 am_irradiance = irradianceMap->sampleCubeMap(norm);
		vec3 am_diffuse = am_irradiance * albedo;

		// specular
		vec3 prefilteredColor = prefilterMap->sampleCubeMap(reflectDir);
		vec3 brdfV3 = brdfLUT->sampleTex(vec2(nvDot, roughness));
		vec2 brdf = vec2(brdfV3.x, brdfV3.y);
		vec3 am_specular = prefilteredColor * (am_ks * brdf.x + brdf.y);

		vec3 ambient = (am_kd * am_diffuse + am_specular) * ao;

		/**************************************************/

		vec3 color = ambient + l;

		// reinhard HDR tonemapping
		color = color / (color + vec3(1.0));
		// gamma correct
		color = pow(color, vec3(1.0 / 2.2));

		return color;
	}

private:

	// Fresnel Equation with roughness for ambient lighting
	vec3 fresnelEquationRoughness(vec3 n, vec3 v, vec3 F0, float a) {
		float nvDot = maxInTwo(dot(n, v), 0);
		return F0 + (maxInTwo(vec3(1.0 - a), F0) - F0) * pow(1.0 - nvDot, 5.0);
	}
};