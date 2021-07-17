#include "clip.h"

std::vector<VS_out*> Clip(std::vector<VS_out*> payloads) {
	std::vector<VS_out*> result = payloads;
	for (int i = 0; i < 3; i++) {
		result = ClipPolygonComponent(result, i, 1.0f);
		result = ClipPolygonComponent(result, i, -1.0f);
	}

	return result;
}

std::vector<VS_out*> ClipPolygonComponent(std::vector<VS_out*> payloads, int axis, float sign) {
	std::vector<VS_out*> result;
	if (payloads.empty()) { // empty vertices
		return result;
	}

	VS_out* pPayload = payloads[payloads.size() - 1]; // previous, current
	float pComponent, cComponent;
	switch (axis) {  // 0~2 => x~z
		case 0:
			pComponent = pPayload->pos.x * sign; // positive semi-axis and negative semi-axis
			break;
		case 1:
			pComponent = pPayload->pos.y * sign;
			break;
		case 2:
			pComponent = pPayload->pos.z * sign;
			break;
		default:
			std::cout << "clip axis index error" << std::endl;
			return result;
	}
	bool pInside = pComponent <= pPayload->pos.w, cInside;

	for (int i = 0; i < payloads.size(); i++) {
		VS_out* cPayload = payloads[i];
		switch (axis) {
		case 0:
			cComponent = cPayload->pos.x * sign;
			break;
		case 1:
			cComponent = cPayload->pos.y * sign;
			break;
		case 2:
			cComponent = cPayload->pos.z * sign;
			break;
		default:
			std::cout << "clip axis index error" << std::endl;
			return result;
		}
		cInside = cComponent <= cPayload->pos.w;

		if (pInside ^ cInside) { // xOR
			float t = (pPayload->pos.w - pComponent) / ((pPayload->pos.w - pComponent) - (cPayload->pos.w - cComponent));
			result.push_back(lerp(pPayload, cPayload, t));
		}

		if (cInside) {
			result.push_back(cPayload);
		}

		delete pPayload;

		pPayload = cPayload;
		pComponent = cComponent;
		pInside = cInside;
	}
	
	return result;
}