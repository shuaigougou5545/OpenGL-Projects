#ifndef Model_h
#define Model_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <cstdint>


struct vertice{
    glm::vec3 Pos;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec2 TexC;
};


class BoundingBox{
public:
    glm::vec3 Center;
    glm::vec3 Extents;
};


class Model{
public:
    std::vector<vertice> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 vMin, vMax;
    BoundingBox bounds;
    
    Model(const std::string& modelPath);
    ~Model();
    
    // for OpenGL VBO data
    // PosX PosY PosZ NormalX NormalY NormalZ TexCX TexCY
    std::vector<float> vertices_vbo;
private:
    
};

#endif /* Model_h */
