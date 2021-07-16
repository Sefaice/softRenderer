#pragma once

#include "./utils/math.h"
#include "./utils/shader_payload.h"

#include <vector>

std::vector<VS_out> Clip(std::vector<VS_out> vertices);

std::vector<VS_out> ClipPolygonComponent(std::vector<VS_out> vertices, int axis, float sign);