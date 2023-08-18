#ifndef ShapeGenerator_h
#define ShapeGenerator_h


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


class Shape{
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};


#endif /* ShapeGenerator_h */
