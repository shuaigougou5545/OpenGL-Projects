#ifndef Light_h
#define Light_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Light{
    glm::vec3 position;
    glm::vec3 color;
};


#endif /* Light_h */
