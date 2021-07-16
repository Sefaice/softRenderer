#pragma once

#include "math.h"

#include <vector>

// vertex shader input
struct VS_in { 
	std::vector<vec2> in_vec2;
	std::vector<vec3> in_vec3;
};

// vertex shader output 
struct VS_out {
	// gl_Position
	vec4 pos;
	// attributes
	std::vector<vec2> out_vec2;
	std::vector<vec3> out_vec3;
	std::vector<mat3> out_mat3;
};

// fragment shader input
struct FS_in {
	// attributes
	std::vector<vec2> in_vec2;
	std::vector<vec3> in_vec3;
	std::vector<mat3> in_mat3;
};

inline VS_out lerp(VS_out v1, VS_out v2, float t) {

	VS_out v;

	v.pos = lerp(v1.pos, v2.pos, t);

	for (unsigned int i = 0; i < v1.out_vec2.size(); i++) {
		v.out_vec2.push_back(lerp(v1.out_vec2[i], v2.out_vec2[i], t));
	}
	for (unsigned int i = 0; i < v1.out_vec3.size(); i++) {
		v.out_vec3.push_back(lerp(v1.out_vec3[i], v2.out_vec3[i], t));
	}
	for (unsigned int i = 0; i < v1.out_mat3.size(); i++) {
		v.out_mat3.push_back(lerp(v1.out_mat3[i], v2.out_mat3[i], t));
	}

	return v;
}

// perspective-correct attributes interpolation
inline FS_in pc_interpolation(VS_out v1, VS_out v2, VS_out v3, double z, double inte_tmp1, double inte_tmp2, double inte_tmp3) {

	FS_in result;

	for (unsigned int i = 0; i < v1.out_vec2.size(); i++) {
		vec2 v1_t = v1.out_vec2[i];
		vec2 v2_t = v2.out_vec2[i];
		vec2 v3_t = v3.out_vec2[i];
		vec2 r_t;
		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
		result.in_vec2.push_back(r_t);
	}
	for (unsigned int i = 0; i < v1.out_vec3.size(); i++) {
		vec3 v1_t = v1.out_vec3[i];
		vec3 v2_t = v2.out_vec3[i];
		vec3 v3_t = v3.out_vec3[i];
		vec3 r_t;
		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
		r_t.z = z * (v1_t.z * inte_tmp1 + v2_t.z * inte_tmp2 + v3_t.z * inte_tmp3);
		result.in_vec3.push_back(r_t);
	}
	for (unsigned int i = 0; i < v1.out_mat3.size(); i++) {
		mat3 v1_t = v1.out_mat3[i];
		mat3 v2_t = v2.out_mat3[i];
		mat3 v3_t = v3.out_mat3[i];
		mat3 r_t;

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				r_t.m[i][j] = z * (v1_t.m[i][j] * inte_tmp1 + v2_t.m[i][j] * inte_tmp2 + v3_t.m[i][j] * inte_tmp3);

		result.in_mat3.push_back(r_t);
	}

	return result;
}