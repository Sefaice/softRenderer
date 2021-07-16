#pragma once

#include "math.h"

struct Vertex {
	vec3 pos;
	vec3 normal;
	vec2 texCoords;
	vec3 tangent;
	vec3 bitangent;

	Vertex() : pos(vec3()), normal(vec3()), texCoords(vec2()), tangent(vec3()), bitangent(vec3()) {}

	//Vertex(vec3 _pos, vec3 _normal, vec2 _texCoords) : pos(_pos), normal(_normal), texCoords(_texCoords) {}
	
	void print() {
		std::cout << "Vertex pos: ";
		pos.print();
		std::cout << "Vertex normal: ";
		normal.print();
		std::cout << "Vertex texture coords: ";
		texCoords.print();
	}
};