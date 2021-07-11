#pragma once

#include "vertex.h"
#include <vector>

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;
        //this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        //setupMesh();
    }
};