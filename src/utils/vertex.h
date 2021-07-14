#pragma once

#include "math.h"

struct Vertex {
	// used for MVP transformations
	vec4 pos; // v shader output, f shader input
	// attributes to be interpolated
	vec3 normal;
	vec2 texCoords;
	vec3 worldPos; // v shader input as localPos, then as worldpos attribute
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN;
	vec3 cubeMapTexCoords;

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
	Vertex v(lerp(v1.pos, v2.pos, t),
		lerp(v1.normal, v2.normal, t), lerp(v1.texCoords, v2.texCoords, t), lerp(v1.worldPos, v2.worldPos, t));

	v.tangent = lerp(v1.tangent, v2.tangent, t);
	v.bitangent = lerp(v1.bitangent, v2.bitangent, t);
	v.TBN = lerp(v1.TBN, v2.TBN, t);
	v.cubeMapTexCoords = lerp(v1.cubeMapTexCoords, v2.cubeMapTexCoords, t);

	return v;
}