#ifndef Model_h
#define Model_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <cstdint>


class BoundingBox{
public:
    glm::vec3 Center;
    glm::vec3 Extents;
};


class Model{
public:
    std::vector<unsigned int> indices;
    
    std::vector<glm::vec3> pos_list;
    std::vector<glm::vec3> normal_list;
    std::vector<glm::vec3> tangent_list;
    std::vector<glm::vec2> texc_list;
    
    glm::vec3 vMin, vMax;
    BoundingBox bounds;
    
    Model(const std::string& modelPath);
    ~Model();
private:
    
};

#endif /* Model_h */
