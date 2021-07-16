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

    void Draw(Raster3d* raster3d, VertexShader* vertexShader, ObjFragmentShader* fragmentShader) {

        // bind texture
        for (Texture* texture : textures) {
            //std::cout << "mesh draw: " <<texture->type << " " << texture->path << std::endl;
            if (texture->type == "texture_diffuse") {
                fragmentShader->diffuseMap = texture;
            }
            else if (texture->type == "texture_specular") {
                fragmentShader->specularMap = texture;
            }
            else if (texture->type == "texture_normal") {
                fragmentShader->normalMap = texture;
            }
            else {
                std::cout << "height map and others not implemented yet." << std::endl;
            }
        }

        for (std::vector<unsigned int>::iterator iter = indices.begin(); iter != indices.end(); iter+=3) {
            Vertex v1 = vertices[*iter];
            Vertex v2 = vertices[*(iter+1)];
            Vertex v3 = vertices[*(iter+2)];

            VS_in vin1;
            vin1.in_vec2 = { v1.texCoords };
            vin1.in_vec3 = { v1.pos, v1.normal, v1.tangent, v1.bitangent };
            VS_in vin2;
            vin2.in_vec2 = { v2.texCoords };
            vin2.in_vec3 = { v2.pos, v2.normal, v2.tangent, v2.bitangent };
            VS_in vin3;
            vin3.in_vec2 = { v3.texCoords };
            vin3.in_vec3 = { v3.pos, v3.normal, v3.tangent, v3.bitangent };

            raster3d->DrawTriangle3D(vin1, vin2, vin3, vertexShader, fragmentShader);
        }
    }
};