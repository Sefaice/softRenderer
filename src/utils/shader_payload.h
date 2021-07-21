#pragma once

#include "math.h"

#include <vector>

// vertex shader input
struct VS_in {
	vec2 texCoords;
	vec3 localPos;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
};

// vertex shader output 
struct VS_out {
	// gl_Position
	vec4 pos;
	// attributes:
	vec2 texCoords;
	vec3 normal;
	vec3 worldPos;
	vec3 cubeMapTexCoords;
	vec4 posLightSpace; // for shadow
	mat3 TBN;
};

// fragment shader input
struct FS_in {
	// attributes
	vec2 texCoords;
	vec3 normal;
	vec3 worldPos;
	vec3 cubeMapTexCoords;
	vec4 posLightSpace;
	mat3 TBN;
};

inline VS_out lerp(VS_out v1, VS_out v2, float t) {

	VS_out v;

	v.pos = lerp(v1.pos, v2.pos, t);
	v.texCoords = lerp(v1.texCoords, v2.texCoords, t);
	v.normal = lerp(v1.normal, v2.normal, t);
	v.worldPos = lerp(v1.worldPos, v2.worldPos, t);
	v.cubeMapTexCoords = lerp(v1.cubeMapTexCoords, v2.cubeMapTexCoords, t);
	v.posLightSpace = lerp(v1.posLightSpace, v2.posLightSpace, t);
	v.TBN = lerp(v1.TBN, v2.TBN, t);

	return v;
}

// perspective-correct attributes interpolation
inline FS_in pc_interpolation(VS_out v1, VS_out v2, VS_out v3, double inte_d, double inte_tmp1, double inte_tmp2, double inte_tmp3) {

	FS_in result;

	result.texCoords.x = inte_d * (v1.texCoords.x * inte_tmp1 + v2.texCoords.x * inte_tmp2 + v3.texCoords.x * inte_tmp3);
	result.texCoords.y = inte_d * (v1.texCoords.y * inte_tmp1 + v2.texCoords.y * inte_tmp2 + v3.texCoords.y * inte_tmp3);

	result.normal.x = inte_d * (v1.normal.x * inte_tmp1 + v2.normal.x * inte_tmp2 + v3.normal.x * inte_tmp3);
	result.normal.y = inte_d * (v1.normal.y * inte_tmp1 + v2.normal.y * inte_tmp2 + v3.normal.y * inte_tmp3);
	result.normal.z = inte_d * (v1.normal.z * inte_tmp1 + v2.normal.z * inte_tmp2 + v3.normal.z * inte_tmp3);
	
	result.worldPos.x = inte_d * (v1.worldPos.x * inte_tmp1 + v2.worldPos.x * inte_tmp2 + v3.worldPos.x * inte_tmp3);
	result.worldPos.y = inte_d * (v1.worldPos.y * inte_tmp1 + v2.worldPos.y * inte_tmp2 + v3.worldPos.y * inte_tmp3);
	result.worldPos.z = inte_d * (v1.worldPos.z * inte_tmp1 + v2.worldPos.z * inte_tmp2 + v3.worldPos.z * inte_tmp3);
	
	result.cubeMapTexCoords.x = inte_d * (v1.cubeMapTexCoords.x * inte_tmp1 + v2.cubeMapTexCoords.x * inte_tmp2 + v3.cubeMapTexCoords.x * inte_tmp3);
	result.cubeMapTexCoords.y = inte_d * (v1.cubeMapTexCoords.y * inte_tmp1 + v2.cubeMapTexCoords.y * inte_tmp2 + v3.cubeMapTexCoords.y * inte_tmp3);
	result.cubeMapTexCoords.z = inte_d * (v1.cubeMapTexCoords.z * inte_tmp1 + v2.cubeMapTexCoords.z * inte_tmp2 + v3.cubeMapTexCoords.z * inte_tmp3);

	result.posLightSpace.x = inte_d * (v1.posLightSpace.x * inte_tmp1 + v2.posLightSpace.x * inte_tmp2 + v3.posLightSpace.x * inte_tmp3);
	result.posLightSpace.y = inte_d * (v1.posLightSpace.y * inte_tmp1 + v2.posLightSpace.y * inte_tmp2 + v3.posLightSpace.y * inte_tmp3);
	result.posLightSpace.z = inte_d * (v1.posLightSpace.z * inte_tmp1 + v2.posLightSpace.z * inte_tmp2 + v3.posLightSpace.z * inte_tmp3);
	result.posLightSpace.w = inte_d * (v1.posLightSpace.w * inte_tmp1 + v2.posLightSpace.w * inte_tmp2 + v3.posLightSpace.w * inte_tmp3);

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			result.TBN.m[i][j] = inte_d * (v1.TBN.m[i][j] * inte_tmp1 + v2.TBN.m[i][j] * inte_tmp2 + v3.TBN.m[i][j] * inte_tmp3);

	return result;
}
