#pragma once

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iomanip>

#define M_PI 3.14159265358979323846f

struct vec2 {
	int x, y;

	vec2() : x(0), y(0) {}
	vec2(int _x, int _y) : x(_x), y(_y) {}
};

struct vec3 {
	float x, y, z;

	vec3() : x(0), y(0), z(0) {}
	vec3(float _a) : x(_a), y(_a), z(_a) {}
	vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "vec3(" << x << ", " << y << ", " << z << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
	float length() {
		return sqrt(x * x + y * y + z * z);
	}
};

struct vec4 {
	float x, y, z, w;

	vec4() : x(0), y(0), z(0), w(0) {}
	vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	vec4(vec3 _v, float _w) : x(_v.x), y(_v.y), z(_v.z), w(_w) {}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "vec4(" << x << ", " << y << ", " << z << ", " << w << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
};

struct Vertex {
	vec4 pos;
	vec3 color;

	Vertex() : pos(vec4()), color(vec3()) {}
	Vertex(vec4 _pos, vec3 _color) : pos(_pos), color(_color) {}
	void print() {
		std::cout << "Vertex pos: ";
		pos.print();
		std::cout << "Vertex color: ";
		color.print();
	}
};

struct Triangle {
	vec3 pos[3];
	vec3 color[3];
};

struct mat4 {  // column-major. m[0] is actually the first column of matrix 
	float m[4][4];

	mat4() {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m[i][j] = 0;
	}
	mat4(float _x) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = 0;
			}
			m[i][i] = _x;
		}
	}
	mat4(float _x1, float _x2, float _x3, float _x4,
		float _x5, float _x6, float _x7, float _x8,
		float _x9, float _x10, float _x11, float _x12,
		float _x13, float _x14, float _x15, float _x16) {
		m[0][0] = _x1; m[0][1] = _x2; m[0][2] = _x3; m[0][3] = _x4;
		m[1][0] = _x5; m[1][1] = _x6; m[1][2] = _x7; m[1][3] = _x8;
		m[2][0] = _x9; m[2][1] = _x10; m[2][2] = _x11; m[2][3] = _x12;
		m[3][0] = _x13; m[3][1] = _x14; m[3][2] = _x15; m[3][3] = _x16;
	}
	void print() {
		std::cout << std::fixed << std::setprecision(3);
		std::cout << "mat4x4(";
		for (int i = 0; i < 4; i++) {
			std::cout << "(" << m[i][0] << ", " << m[i][1] << ", " << m[i][2] << ", " << m[i][3] << ")";
			if (i != 3) std::cout << ", ";
		}
		std::cout << ")" << std::endl;
		std::cout << std::setprecision(0);
	}
};

inline vec3 normalize(vec3& v) {
	float length = v.length();
	if (length == 0)
		return vec3(99999, 99999, 99999);
	return vec3(v.x / length, v.y / length, v.z / length);
}

inline vec4 operator*(const mat4& m, const vec4& v) {
	return vec4(m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w,
		m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w,
		m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w,
		m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w);
}

inline mat4 operator*(const mat4& m, float a) {
	mat4 r;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = m.m[i][j] * a;
		}
	}

	return r;
}

inline mat4 operator*(const mat4& m1, const mat4& m2) {
	mat4 r;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}

	return r;
}

inline vec3 lerp(vec3 v1, vec3 v2, float t) { // v3 = v1 * (1 - t) + v2 * t
	return vec3(v1.x * (1 - t) + v2.x * t, v1.y * (1 - t) + v2.y * t,
		v1.z * (1 - t) + v2.z * t);
}

inline vec4 lerp(vec4 v1, vec4 v2, float t) {
	return vec4(v1.x * (1 - t) + v2.x * t, v1.y * (1 - t) + v2.y * t,
		v1.z * (1 - t) + v2.z * t, v1.w * (1 - t) + v2.w * t);
}

inline Vertex lerp(Vertex v1, Vertex v2, float t) {
	return Vertex(lerp(v1.pos, v2.pos, t), lerp(v1.color, v2.color, t));
}

inline float radians(float degree) {
	return degree * M_PI / 180.0f;
}

inline mat4 translate(const mat4& m, const vec3& v) {
	mat4 t = mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		v.x, v.y, v.z, 1.0);
	
	return t * m;
}

inline mat4 rotate(const mat4& m, float angle, vec3 axis) { // angle is in [0, 360]
	axis = normalize(axis);
	float a = cos(angle / 2);
	float b = sin(angle / 2) * axis.x;
	float c = sin(angle / 2) * axis.y;
	float d = sin(angle / 2) * axis.z;
	mat4 r = mat4(1 - 2 * c * c - 2 * d * d, 2 * b * c + 2 * a * d,     2 * b * d - 2 * a * c,     0,
				  2 * b * c - 2 * a * d,     1 - 2 * b * b - 2 * d * d, 2 * a * b + 2 * c * d,     0,
				  2 * a * c + 2 * b * d,     2 * c * d - 2 * a * b,     1 - 2 * b * b - 2 * c * c, 0,
				  0,                         0,                         0,                         1);

	return r * m;
}

inline mat4 scale(const mat4& m, float scaleAmount) {
	mat4 s = mat4(scaleAmount, 0, 0, 0,
		0, scaleAmount, 0, 0,
		0, 0, scaleAmount, 0,
		0, 0, 0, 1);

	return s * m;
}