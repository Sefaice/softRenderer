#pragma once

#include "../utils/math.h"
#include "../utils/vertex.h"

class VertexShader {
public:
	mat4 model;
	mat4 view;
	mat4 projection;

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

	virtual Vertex shading(Vertex v) { return v; }
};

class PhongVertexShader : public VertexShader {
public:

	/*Vertex vShader(vec3 p, vec3 n, vec2 t) {
		vec4 pp = MVP_transform(p);
		vec3 pw = getWorldPos(p);
		vec3 n_world = matrix3(transpose(inverse(model))) * n;

		return Vertex(pp, n_world, t, pw);
	}*/
	Vertex shading(Vertex v) {

		vec3 T = vector3(model * vec4(v.tangent, 0.0));
		T = normalize(T);
		vec3 B = vector3(model * vec4(v.bitangent, 0.0));
		B = normalize(B);
		vec3 N = vector3(model * vec4(v.normal, 0.0));
		N = normalize(N);
		//// in frag shader use TBN
		//mat3 TBN = mat3(T.x, T.y, T.z, B.x, B.y, B.z, N.x, N.y, N.z);
		//v.TBN = TBN;

		v.pos = MVP_transform(v.worldPos);
		v.worldPos = getWorldPos(v.worldPos);
		v.normal = matrix3(transpose(inverse(model))) * v.normal;
		return v;
	}
};

class CubeMapVertexShader : public VertexShader {
public:

	Vertex shading(Vertex v) {

		v.pos = projection *view * vec4(v.worldPos, 1.0);
		v.cubeMapTexCoords = v.worldPos; // actually localPos
		
		// The resulting normalized device coordinates will then always have a z value equal to 1.0: the maximum depth value
		v.pos.z = v.pos.w;

		return v;
	}
};