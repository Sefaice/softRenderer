#pragma once

#include "math.h"

struct Vertex {
	// used for MVP transformations
	vec4 pos;
	// attributes to be interpolated
	vec3 normal;
	vec2 texCoords;
	vec3 worldPos;
	vec3 tangent;
	vec3 bitangent;

	Vertex() : pos(vec4()), normal(vec3()), texCoords(vec2()), worldPos(vec3()) {}

	Vertex(vec4 _pos, vec3 _normal, vec2 _texCoords, vec3 _worldPos) : pos(_pos), normal(_normal), texCoords(_texCoords), worldPos(_worldPos) {}
	
	void print() {
		std::cout << "Vertex pos: ";
		pos.print();
		std::cout << "Vertex normal: ";
		normal.print();
		std::cout << "Vertex texture coords: ";
		texCoords.print();
		std::cout << "Vertex world position: ";
		worldPos.print();
	}
};

inline Vertex lerp(Vertex v1, Vertex v2, float t) {
	return Vertex(lerp(v1.pos, v2.pos, t), lerp(v1.normal, v2.normal, t), lerp(v1.texCoords, v2.texCoords, t),
		lerp(v1.worldPos, v2.worldPos, t));
}

//struct Triangle {
//	vec3 pos[3];
//	vec2 texCoords[3];
//	vec3 normal[3];
//};