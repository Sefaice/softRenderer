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

	virtual VS_out shading(VS_in v) {
		std::cout << "error! virtual v shading" << std::endl;
		return VS_out();
	}
};

class PhongVertexShader : public VertexShader {
	// VS_in:
	//     vec3 localPos
	//     vec3 normal
	//     vec2 texCoords
	// VS_out:
	//	   vec4 pos
	//     vec3 normal
	//     vec3 worldPos
	//     vec2 texCoords
public:
	VS_out shading(VS_in vin) {

		vec3 localPos = vin.in_vec3[0];
		vec3 normal = vin.in_vec3[1];
		vec2 texCoords = vin.in_vec2[0];

		vec4 pos = MVP_transform(localPos);
		vec3 worldPos = getWorldPos(localPos);
		vec3 worldNormal = matrix3(transpose(inverse(model))) * normal;

		VS_out vout;
		vout.pos = pos;
		vout.out_vec3.push_back(worldNormal);
		vout.out_vec3.push_back(worldPos);
		vout.out_vec2.push_back(texCoords);

		return vout;
	}
};

class ObjVertexShader : public VertexShader {
	// VS_in:
	//     vec3 localPos
	//     vec3 normal
	//     vec3 tangent
	//     vec3 bitangent
	//     vec2 texCoords
	// VS_out:
	//	   vec4 pos
	//     vec3 normal
	//     vec3 worldPos
	//     vec2 texCoords
	//     mat3 TBN
public:
	VS_out shading(VS_in vin) {

		vec3 localPos = vin.in_vec3[0];
		vec3 normal = vin.in_vec3[1];
		vec3 tangent = vin.in_vec3[2];
		vec3 bitangent = vin.in_vec3[3];
		vec2 texCoords = vin.in_vec2[0];

		vec3 T = vector3(model * vec4(tangent, 0.0));
		T = normalize(T);
		vec3 B = vector3(model * vec4(bitangent, 0.0));
		B = normalize(B);
		vec3 N = vector3(model * vec4(normal, 0.0));
		N = normalize(N);
		// in frag shader use TBN
		mat3 TBN = mat3(T.x, T.y, T.z, B.x, B.y, B.z, N.x, N.y, N.z);

		vec4 pos = MVP_transform(localPos);
		vec3 worldPos = getWorldPos(localPos);
		vec3 worldNormal = matrix3(transpose(inverse(model))) * normal;

		VS_out vout;
		vout.pos = pos;
		vout.out_vec3.push_back(worldNormal);
		vout.out_vec3.push_back(worldPos);
		vout.out_vec2.push_back(texCoords);
		vout.out_mat3.push_back(TBN);

		return vout;
	}
};

class CubeMapVertexShader : public VertexShader {
	// VS_in:
	//     vec3 localPos   
	// VS_out:
	//	   vec4 pos
	//     vec3 cubeMapTexCoords
public:
	VS_out shading(VS_in vin) {

		vec3 localPos = vin.in_vec3[0];

		vec4 pos = projection *view * vec4(localPos, 1.0);
		vec3 cubeMapTexCoords = localPos; // actually localPos
		
		// The resulting normalized device coordinates will then always have a z value equal to 1.0: the maximum depth value
		pos.z = pos.w;

		VS_out vout;
		vout.pos = pos;
		vout.out_vec3.push_back(cubeMapTexCoords);

		return vout;
	}
};