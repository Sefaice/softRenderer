#pragma once

#include "math.h"

#include <vector>

// vertex shader input
class VS_in {
public:
	vec2* in_vec2;
	vec3* in_vec3;
	unsigned int num_in_vec2;
	unsigned int num_in_vec3;

	VS_in(unsigned int _num_in_vec2, unsigned int _num_in_vec3) {
		num_in_vec2 = _num_in_vec2;
		num_in_vec3 = _num_in_vec3;
		in_vec2 = new vec2[num_in_vec2];
		in_vec3 = new vec3[num_in_vec3];
	}

	VS_in(const VS_in& v) {
		num_in_vec2 = v.num_in_vec2;
		num_in_vec3 = v.num_in_vec3;
		in_vec2 = new vec2[num_in_vec2];
		in_vec3 = new vec3[num_in_vec3];
		for (unsigned int i = 0; i < num_in_vec2; i++) { // can change to memcpy
			in_vec2[i] = v.in_vec2[i];
		}
		for (unsigned int i = 0; i < num_in_vec3; i++) {
			in_vec3[i] = v.in_vec3[i];
		}
	}

	VS_in& operator=(const VS_in& v) {
		num_in_vec2 = v.num_in_vec2;
		num_in_vec3 = v.num_in_vec3;
		in_vec2 = new vec2[num_in_vec2];
		in_vec3 = new vec3[num_in_vec3];
		for (unsigned int i = 0; i < num_in_vec2; i++) {
			in_vec2[i] = v.in_vec2[i];
		}
		for (unsigned int i = 0; i < num_in_vec3; i++) {
			in_vec3[i] = v.in_vec3[i];
		}

		return *this;
	}

	VS_in(vec2 texcoords, vec3 pos, vec3 normal) {
		num_in_vec2 = 1;
		num_in_vec3 = 2;
		in_vec2 = new vec2[1];
		in_vec3 = new vec3[2];
		in_vec2[0] = texcoords;
		in_vec3[0] = pos; 
		in_vec3[1] = normal;
	}

	~VS_in() {
		delete in_vec2;
		delete in_vec3;
	}
};

// vertex shader output 
class VS_out {
public:
	// gl_Position
	vec4 pos;
	// attributes
	vec2* out_vec2;
	vec3* out_vec3;
	mat3* out_mat3;
	unsigned int num_out_vec2;
	unsigned int num_out_vec3;
	unsigned int num_out_mat3;

	VS_out(unsigned int _num_out_vec2, unsigned int _num_out_vec3, unsigned int _num_out_mat3) {
		num_out_vec2 = _num_out_vec2;
		num_out_vec3 = _num_out_vec3;
		num_out_mat3 = _num_out_mat3;
		out_vec2 = new vec2[num_out_vec2];
		out_vec3 = new vec3[num_out_vec3];
		out_mat3 = new mat3[num_out_mat3];
	}

	VS_out(const VS_out& v) {
		std::cout << "copy cons" << std::endl;
		pos = v.pos;

		num_out_vec2 = v.num_out_vec2;
		num_out_vec3 = v.num_out_vec3;
		num_out_mat3 = v.num_out_mat3;
		out_vec2 = new vec2[num_out_vec2];
		out_vec3 = new vec3[num_out_vec3];
		out_mat3 = new mat3[num_out_mat3];
		for (unsigned int i = 0; i < num_out_vec2; i++) {
			out_vec2[i] = v.out_vec2[i];
		}
		for (unsigned int i = 0; i < num_out_vec3; i++) {
			out_vec3[i] = v.out_vec3[i];
		}
		for (unsigned int i = 0; i < num_out_mat3; i++) {
			out_mat3[i] = v.out_mat3[i];
		}
	}

	VS_out& operator=(const VS_out& v) {
		std::cout << "assign cons" << std::endl;
		pos = v.pos;

		num_out_vec2 = v.num_out_vec2;
		num_out_vec3 = v.num_out_vec3;
		num_out_mat3 = v.num_out_mat3;
		out_vec2 = new vec2[num_out_vec2];
		out_vec3 = new vec3[num_out_vec3];
		out_mat3 = new mat3[num_out_mat3];
		for (unsigned int i = 0; i < num_out_vec2; i++) {
			out_vec2[i] = v.out_vec2[i];
		}
		for (unsigned int i = 0; i < num_out_vec3; i++) {
			out_vec3[i] = v.out_vec3[i];
		}
		for (unsigned int i = 0; i < num_out_mat3; i++) {
			out_mat3[i] = v.out_mat3[i];
		}

		return *this;
	}

	~VS_out() {
		delete out_vec2;
		delete out_vec3;
		delete out_mat3;
	}
};

// fragment shader input
class FS_in {
public:
	// attributes
	vec2* in_vec2;
	vec3* in_vec3;
	mat3* in_mat3;
	unsigned int num_in_vec2;
	unsigned int num_in_vec3;
	unsigned int num_in_mat3;

	FS_in(unsigned int _num_in_vec2, unsigned int _num_in_vec3, unsigned int _num_in_mat3) {
		num_in_vec2 = _num_in_vec2;
		num_in_vec3 = _num_in_vec3;
		num_in_mat3 = _num_in_mat3;
		in_vec2 = new vec2[num_in_vec2];
		in_vec3 = new vec3[num_in_vec3];
		in_mat3 = new mat3[num_in_mat3];
	}

	FS_in(const FS_in& f) {
		num_in_vec2 = f.num_in_vec2;
		num_in_vec3 = f.num_in_vec3;
		num_in_mat3 = f.num_in_mat3;
		in_vec2 = new vec2[num_in_vec2];
		in_vec3 = new vec3[num_in_vec3];
		in_mat3 = new mat3[num_in_mat3];
		for (unsigned int i = 0; i < num_in_vec2; i++) {
			in_vec2[i] = f.in_vec2[i];
		}
		for (unsigned int i = 0; i < num_in_vec3; i++) {
			in_vec3[i] = f.in_vec3[i];
		}
		for (unsigned int i = 0; i < num_in_mat3; i++) {
			in_mat3[i] = f.in_mat3[i];
		}
	}

	FS_in& operator=(const FS_in& f) {
		num_in_vec2 = f.num_in_vec2;
		num_in_vec3 = f.num_in_vec3;
		num_in_mat3 = f.num_in_mat3;
		in_vec2 = new vec2[num_in_vec2];
		in_vec3 = new vec3[num_in_vec3];
		in_mat3 = new mat3[num_in_mat3];
		for (unsigned int i = 0; i < num_in_vec2; i++) {
			in_vec2[i] = f.in_vec2[i];
		}
		for (unsigned int i = 0; i < num_in_vec3; i++) {
			in_vec3[i] = f.in_vec3[i];
		}
		for (unsigned int i = 0; i < num_in_mat3; i++) {
			in_mat3[i] = f.in_mat3[i];
		}

		return *this;
	}

	~FS_in() {
		delete in_vec2;
		delete in_vec3;
		delete in_mat3;
	}
};

inline VS_out lerp(VS_out v1, VS_out v2, float t) {

	VS_out v(v1.num_out_vec2, v1.num_out_vec3, v1.num_out_mat3);

	v.pos = lerp(v1.pos, v2.pos, t);

	for (unsigned int i = 0; i < v1.num_out_vec2; i++) {
		v.out_vec2[i] = lerp(v1.out_vec2[i], v2.out_vec2[i], t);
	}
	for (unsigned int i = 0; i < v1.num_out_vec3; i++) {
		v.out_vec3[i] = lerp(v1.out_vec3[i], v2.out_vec3[i], t);
	}
	for (unsigned int i = 0; i < v1.num_out_mat3; i++) {
		v.out_mat3[i] = lerp(v1.out_mat3[i], v2.out_mat3[i], t);
	}

	return v;
}

inline VS_out* lerp(VS_out* v1, VS_out* v2, float t) {

	VS_out* v = new VS_out(v1->num_out_vec2, v1->num_out_vec3, v1->num_out_mat3);

	v->pos = lerp(v1->pos, v2->pos, t);

	for (unsigned int i = 0; i < v1->num_out_vec2; i++) {
		v->out_vec2[i] = lerp(v1->out_vec2[i], v2->out_vec2[i], t);
	}
	for (unsigned int i = 0; i < v1->num_out_vec3; i++) {
		v->out_vec3[i] = lerp(v1->out_vec3[i], v2->out_vec3[i], t);
	}
	for (unsigned int i = 0; i < v1->num_out_mat3; i++) {
		v->out_mat3[i] = lerp(v1->out_mat3[i], v2->out_mat3[i], t);
	}

	return v;
}

// perspective-correct attributes interpolation
inline FS_in pc_interpolation(VS_out v1, VS_out v2, VS_out v3, double z, double inte_tmp1, double inte_tmp2, double inte_tmp3) {

	FS_in result(v1.num_out_vec2, v1.num_out_vec3, v1.num_out_mat3);

	for (unsigned int i = 0; i < v1.num_out_vec2; i++) {
		vec2 v1_t = v1.out_vec2[i];
		vec2 v2_t = v2.out_vec2[i];
		vec2 v3_t = v3.out_vec2[i];
		vec2 r_t;
		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
		result.in_vec2[i] =r_t;
	}
	for (unsigned int i = 0; i < v1.num_out_vec3; i++) {
		vec3 v1_t = v1.out_vec3[i];
		vec3 v2_t = v2.out_vec3[i];
		vec3 v3_t = v3.out_vec3[i];
		vec3 r_t;
		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
		r_t.z = z * (v1_t.z * inte_tmp1 + v2_t.z * inte_tmp2 + v3_t.z * inte_tmp3);
		result.in_vec3[i] = r_t;
	}
	for (unsigned int i = 0; i < v1.num_out_mat3; i++) {
		mat3 v1_t = v1.out_mat3[i];
		mat3 v2_t = v2.out_mat3[i];
		mat3 v3_t = v3.out_mat3[i];
		mat3 r_t;

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				r_t.m[i][j] = z * (v1_t.m[i][j] * inte_tmp1 + v2_t.m[i][j] * inte_tmp2 + v3_t.m[i][j] * inte_tmp3);

		result.in_mat3[i] = r_t;
	}

	return result;
}

inline FS_in pc_interpolation(VS_out* v1, VS_out* v2, VS_out* v3, double z, double inte_tmp1, double inte_tmp2, double inte_tmp3) {

	FS_in result(v1->num_out_vec2, v1->num_out_vec3, v1->num_out_mat3);

	for (unsigned int i = 0; i < v1->num_out_vec2; i++) {
		vec2 v1_t = v1->out_vec2[i];
		vec2 v2_t = v2->out_vec2[i];
		vec2 v3_t = v3->out_vec2[i];
		vec2 r_t;
		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
		result.in_vec2[i] = r_t;
	}
	for (unsigned int i = 0; i < v1->num_out_vec3; i++) {
		vec3 v1_t = v1->out_vec3[i];
		vec3 v2_t = v2->out_vec3[i];
		vec3 v3_t = v3->out_vec3[i];
		vec3 r_t;
		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
		r_t.z = z * (v1_t.z * inte_tmp1 + v2_t.z * inte_tmp2 + v3_t.z * inte_tmp3);
		result.in_vec3[i] = r_t;
	}
	for (unsigned int i = 0; i < v1->num_out_mat3; i++) {
		mat3 v1_t = v1->out_mat3[i];
		mat3 v2_t = v2->out_mat3[i];
		mat3 v3_t = v3->out_mat3[i];
		mat3 r_t;

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				r_t.m[i][j] = z * (v1_t.m[i][j] * inte_tmp1 + v2_t.m[i][j] * inte_tmp2 + v3_t.m[i][j] * inte_tmp3);

		result.in_mat3[i] = r_t;
	}

	return result;
}

//// vertex shader input
//struct VS_in { 
//	std::vector<vec2> in_vec2;
//	std::vector<vec3> in_vec3;
//};
//
//// vertex shader output 
//struct VS_out {
//	// gl_Position
//	vec4 pos;
//	// attributes
//	std::vector<vec2> out_vec2;
//	std::vector<vec3> out_vec3;
//	std::vector<mat3> out_mat3;
//};
//
//// fragment shader input
//struct FS_in {
//	// attributes
//	std::vector<vec2> in_vec2;
//	std::vector<vec3> in_vec3;
//	std::vector<mat3> in_mat3;
//};
//
//inline VS_out lerp(VS_out v1, VS_out v2, float t) {
//
//	VS_out v;
//
//	v.pos = lerp(v1.pos, v2.pos, t);
//
//	for (unsigned int i = 0; i < v1.out_vec2.size(); i++) {
//		v.out_vec2.push_back(lerp(v1.out_vec2[i], v2.out_vec2[i], t));
//	}
//	for (unsigned int i = 0; i < v1.out_vec3.size(); i++) {
//		v.out_vec3.push_back(lerp(v1.out_vec3[i], v2.out_vec3[i], t));
//	}
//	for (unsigned int i = 0; i < v1.out_mat3.size(); i++) {
//		v.out_mat3.push_back(lerp(v1.out_mat3[i], v2.out_mat3[i], t));
//	}
//
//	return v;
//}
//
//// perspective-correct attributes interpolation
//inline FS_in pc_interpolation(VS_out v1, VS_out v2, VS_out v3, double z, double inte_tmp1, double inte_tmp2, double inte_tmp3) {
//
//	FS_in result;
//
//	for (unsigned int i = 0; i < v1.out_vec2.size(); i++) {
//		vec2 v1_t = v1.out_vec2[i];
//		vec2 v2_t = v2.out_vec2[i];
//		vec2 v3_t = v3.out_vec2[i];
//		vec2 r_t;
//		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
//		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
//		result.in_vec2.push_back(r_t);
//	}
//	for (unsigned int i = 0; i < v1.out_vec3.size(); i++) {
//		vec3 v1_t = v1.out_vec3[i];
//		vec3 v2_t = v2.out_vec3[i];
//		vec3 v3_t = v3.out_vec3[i];
//		vec3 r_t;
//		r_t.x = z * (v1_t.x * inte_tmp1 + v2_t.x * inte_tmp2 + v3_t.x * inte_tmp3);
//		r_t.y = z * (v1_t.y * inte_tmp1 + v2_t.y * inte_tmp2 + v3_t.y * inte_tmp3);
//		r_t.z = z * (v1_t.z * inte_tmp1 + v2_t.z * inte_tmp2 + v3_t.z * inte_tmp3);
//		result.in_vec3.push_back(r_t);
//	}
//	for (unsigned int i = 0; i < v1.out_mat3.size(); i++) {
//		mat3 v1_t = v1.out_mat3[i];
//		mat3 v2_t = v2.out_mat3[i];
//		mat3 v3_t = v3.out_mat3[i];
//		mat3 r_t;
//
//		for (int i = 0; i < 3; i++)
//			for (int j = 0; j < 3; j++)
//				r_t.m[i][j] = z * (v1_t.m[i][j] * inte_tmp1 + v2_t.m[i][j] * inte_tmp2 + v3_t.m[i][j] * inte_tmp3);
//
//		result.in_mat3.push_back(r_t);
//	}
//
//	return result;
//}