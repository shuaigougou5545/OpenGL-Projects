#ifndef ShapeGenerator_h
#define ShapeGenerator_h


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>


class Shape{
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class Box : public Shape{
private:
    float length = 1.0; // x
    float width = 1.0; // z
    float height = 1.0; // y
public:
    Box(float _scale) : length(_scale), width(_scale), height(_scale)
    {
        BuildVertices();
    }
    
    Box(float _l, float _w, float _h) : length(_l), width(_w), height(_h)
    {
        BuildVertices();
    }
    
    void BuildVertices()
    {
        vertices.reserve(36 * 6);
        
        float x = length / 2.0f;
        float y = height / 2.0f;
        float z = width / 2.0f;

        vertices = {
            -x, -y, -z,  0.0f,  0.0f, -1.0f,
             x, -y, -z,  0.0f,  0.0f, -1.0f,
             x,  y, -z,  0.0f,  0.0f, -1.0f,
             x,  y, -z,  0.0f,  0.0f, -1.0f,
            -x,  y, -z,  0.0f,  0.0f, -1.0f,
            -x, -y, -z,  0.0f,  0.0f, -1.0f,

            -x, -y,  z,  0.0f,  0.0f,  1.0f,
             x, -y,  z,  0.0f,  0.0f,  1.0f,
             x,  y,  z,  0.0f,  0.0f,  1.0f,
             x,  y,  z,  0.0f,  0.0f,  1.0f,
            -x,  y,  z,  0.0f,  0.0f,  1.0f,
            -x, -y,  z,  0.0f,  0.0f,  1.0f,

            -x,  y,  z, -1.0f,  0.0f,  0.0f,
            -x,  y, -z, -1.0f,  0.0f,  0.0f,
            -x, -y, -z, -1.0f,  0.0f,  0.0f,
            -x, -y, -z, -1.0f,  0.0f,  0.0f,
            -x, -y,  z, -1.0f,  0.0f,  0.0f,
            -x,  y,  z, -1.0f,  0.0f,  0.0f,

             x,  y,  z,  1.0f,  0.0f,  0.0f,
             x,  y, -z,  1.0f,  0.0f,  0.0f,
             x, -y, -z,  1.0f,  0.0f,  0.0f,
             x, -y, -z,  1.0f,  0.0f,  0.0f,
             x, -y,  z,  1.0f,  0.0f,  0.0f,
             x,  y,  z,  1.0f,  0.0f,  0.0f,

            -x, -y, -z,  0.0f, -1.0f,  0.0f,
             x, -y, -z,  0.0f, -1.0f,  0.0f,
             x, -y,  z,  0.0f, -1.0f,  0.0f,
             x, -y,  z,  0.0f, -1.0f,  0.0f,
            -x, -y,  z,  0.0f, -1.0f,  0.0f,
            -x, -y, -z,  0.0f, -1.0f,  0.0f,

            -x,  y, -z,  0.0f,  1.0f,  0.0f,
             x,  y, -z,  0.0f,  1.0f,  0.0f,
             x,  y,  z,  0.0f,  1.0f,  0.0f,
             x,  y,  z,  0.0f,  1.0f,  0.0f,
            -x,  y,  z,  0.0f,  1.0f,  0.0f,
            -x,  y, -z,  0.0f,  1.0f,  0.0f
        };
    }
};


#endif /* ShapeGenerator_h */
