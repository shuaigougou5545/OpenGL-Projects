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

    pos_list.resize(vcount);
    normal_list.resize(vcount);
    tangent_list.resize(vcount);
    texc_list.resize(vcount);
    
    
    for (int i = 0; i < vcount; ++i)
    {
        fin >> pos_list[i].x >> pos_list[i].y >> pos_list[i].z;
        fin >> normal_list[i].x >> normal_list[i].y >> normal_list[i].z;

        texc_list[i] = glm::vec2(0.f, 0.f);

        auto N = normal_list[i];
        auto up = glm::vec3(0.0f, 1.0f, 0.0f);

        if(glm::dot(N, up) < 1.0f - 0.001f){
            auto T = glm::normalize(glm::cross(up, N));
            tangent_list[i] = T;
        }
        else
        {
            up = glm::vec3(0.f, 0.f, 1.f);
            auto T = glm::normalize(glm::cross(N, up));
            tangent_list[i] = T;
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
}

Model::~Model()
{
      
}
