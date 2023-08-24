#include "Model.h"
#include <iostream>
#include <fstream>


Model::Model(const std::string& modelPath)
{
    std::ifstream fin(modelPath.c_str());

    if (!fin)
    {
        std::cout << "ERROR [Model]: load model '" << modelPath << "' failed" << std::endl;
        return;
    }

    int vcount = 0;
    int tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;

    vMin = glm::vec3(std::numeric_limits<float>::max());
    vMax = glm::vec3(std::numeric_limits<float>::min());

    vertices.resize(vcount);
    for (int i = 0; i < vcount; ++i)
    {
        fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
        fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

        vertices[i].TexC = glm::vec2(0.f, 0.f);

        auto N = vertices[i].Normal;
        auto up = glm::vec3(0.0f, 1.0f, 0.0f);

        if(glm::dot(N, up) < 1.0f - 0.001f){
            auto T = glm::normalize(glm::cross(up, N));
            vertices[i].Tangent = T;
        }
        else
        {
            up = glm::vec3(0.f, 0.f, 1.f);
            auto T = glm::normalize(glm::cross(N, up));
            vertices[i].Tangent = T;
        }
        
    }

    bounds.Center = 0.5f * (vMin + vMax);
    bounds.Extents = 0.5f * (vMax - vMin);

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    indices.resize(3 * tcount);
    for (int i = 0; i < tcount; ++i)
    {
        fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
    }

    fin.close();
    
    
    // build OpenGL VBO data
    const int elementsPerRow = 6;
    vertices_vbo.resize(vertices.size() * elementsPerRow);
    for(int i = 0; i < vertices.size(); i++)
    {
        vertices_vbo[elementsPerRow * i + 0] = vertices[i].Pos.x;
        vertices_vbo[elementsPerRow * i + 1] = vertices[i].Pos.y;
        vertices_vbo[elementsPerRow * i + 2] = vertices[i].Pos.z;
        vertices_vbo[elementsPerRow * i + 3] = vertices[i].Normal.x;
        vertices_vbo[elementsPerRow * i + 4] = vertices[i].Normal.y;
        vertices_vbo[elementsPerRow * i + 5] = vertices[i].Normal.z;
    }
}

Model::~Model()
{
      
}
