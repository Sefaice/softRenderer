#pragma once

#include "vertex.h"
#include <vector>

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
    std::vector<Texture*> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
    }

    void Draw(Raster3d* raster3d, VertexShader* vertexShader, FragmentShader* fragmentShader) {

        // bind texture
        for (Texture* texture : textures) {
            if (texture->type == "texture_diffuse") {
                fragmentShader->diffuseMap = texture;
            }
            else if (texture->type == "texture_specular") {
                fragmentShader->specularMap = texture;
            }
            else if (texture->type == "texture_normal") {
                fragmentShader->normalMap = texture;
            }
        }

        for (std::vector<unsigned int>::iterator iter = indices.begin(); iter != indices.end(); iter+=3) {
            Vertex v1 = vertices[*iter];
            Vertex v2 = vertices[*(iter+1)];
            Vertex v3 = vertices[*(iter+2)];
            raster3d->DrawTriangle3D(v1.worldPos, v2.worldPos, v3.worldPos, v1.normal, v2.normal, v3.normal,
                v1.texCoords, v2.texCoords, v3.texCoords, vertexShader, fragmentShader);
        }
    }
};