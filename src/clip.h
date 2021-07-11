#pragma once

#include "./utils/math.h"
#include "./utils/vertex.h"
#include <vector>

std::vector<Vertex> Clip(std::vector<Vertex> vertices);

std::vector<Vertex> ClipPolygonComponent(std::vector<Vertex> vertices, int axis, float sign);