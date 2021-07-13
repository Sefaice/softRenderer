#pragma once

#include "./utils/math.h"
#include "./utils/vertex.h"

class VertexShader {
public:
	mat4 model;
	mat4 view;
	mat4 projection;

	VertexShader(mat4 _model, mat4 _view, mat4 _projection) : model(_model), view(_view), projection(_projection) {}

	vec4 MVP_transform(vec3 p) {
		vec4 pl = vec4(p, 1.0); // homogenous coordinates

		// local -> world
		vec4 pw = model * pl;
		// world -> view
		vec4 pv = view * pw;
		// view -> clip(perspective)
		vec4 pp = projection * pv;

		return pp;
	}

	vec3 getWorldPos(vec3 p) {
		vec4 pl = vec4(p, 1.0);
		vec4 pw = model * pl;

		return vec3(pw.x, pw.y, pw.z);
	}

	/*Vertex vShader(vec3 p, vec3 n, vec2 t) {
		vec4 pp = MVP_transform(p);
		vec3 pw = getWorldPos(p);
		vec3 n_world = matrix3(transpose(inverse(model))) * n;

		return Vertex(pp, n_world, t, pw);
	}*/
	Vertex vShader(Vertex v) {

		vec4 T4 = model * vec4(v.tangent, 0.0);
		vec3 T = vec3(T4.x, T4.y, T4.z);
		T = normalize(T);
		vec4 B4 = model * vec4(v.bitangent, 0.0);
		vec3 B = vec3(B4.x, B4.y, B4.z);
		B = normalize(B);
		vec4 N4 = model * vec4(v.normal, 0.0);
		vec3 N = vec3(N4.x, N4.y, N4.z);
		N = normalize(N);
		// in frag shader use TBN
		mat3 TBN = mat3(T.x, T.y, T.z, B.x, B.y, B.z, N.x, N.y, N.z);
		v.TBN = TBN;

		v.pos = MVP_transform(v.worldPos);
		v.worldPos = getWorldPos(v.worldPos);
		v.normal = matrix3(transpose(inverse(model))) * v.normal;

		return v;
	}
};