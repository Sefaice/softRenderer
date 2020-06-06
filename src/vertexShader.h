#pragma once

#include "math.h"

struct VertexShader {
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
	Vertex vShader(vec3 p, vec3 n, vec2 t) {
		vec4 pp = MVP_transform(p);
		vec3 pw = getWorldPos(p);
		vec3 n_world = matrix3(transpose(inverse(model))) * n;

		return Vertex(pp, n_world, t, pw);
	}
};
