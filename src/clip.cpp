#include "clip.h"

std::vector<Vertex> Clip(std::vector<Vertex> vertices) {
	std::vector<Vertex> result = vertices;
	for (int i = 0; i < 3; i++) {
		result = ClipPolygonComponent(result, i, 1.0f);
		result = ClipPolygonComponent(result, i, -1.0f);
	}

	return result;
}

std::vector<Vertex> ClipPolygonComponent(std::vector<Vertex> vertices, int axis, float sign) {
	std::vector<Vertex> result;
	if (vertices.empty()) { // empty vertices
		return result;
	}

	Vertex pVertex = vertices[vertices.size() - 1], cVertex; // previous, current
	float pComponent, cComponent;
	switch (axis) {  // 0~2 => x~z
		case 0:
			pComponent = pVertex.pos.x * sign; // positive semi-axis and negative semi-axis
			break;
		case 1:
			pComponent = pVertex.pos.y * sign;
			break;
		case 2:
			pComponent = pVertex.pos.z * sign;
			break;
		default:
			std::cout << "clip axis index error" << std::endl;
			return result;
	}
	bool pInside = pComponent < pVertex.pos.w, cInside;

	for (int i = 0; i < vertices.size(); i++) {
		cVertex = vertices[i];
		switch (axis) {
		case 0:
			cComponent = cVertex.pos.x * sign;
			break;
		case 1:
			cComponent = cVertex.pos.y * sign;
			break;
		case 2:
			cComponent = cVertex.pos.z * sign;
			break;
		default:
			std::cout << "clip axis index error" << std::endl;
			return result;
		}
		cInside = cComponent < cVertex.pos.w;

		if (pInside ^ cInside) { // xOR
			float t = (pVertex.pos.w - pComponent) / ((pVertex.pos.w - pComponent) - (cVertex.pos.w - cComponent));
			result.push_back(lerp(pVertex, cVertex, t));
		}

		if (cInside) {
			result.push_back(cVertex);
		}

		pVertex = cVertex;
		pComponent = cComponent;
		pInside = cInside;
	}
	
	return result;
}
