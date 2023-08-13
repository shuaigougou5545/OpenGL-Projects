#ifndef ShaderConstructor_h
#define ShaderConstructor_h

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class ShaderConstructor{
public:
    unsigned int ID; // shader program ID
    
    ShaderConstructor(const char* vsPath, const char* fsPath);
    void use();
    void destory();
    
    // set-uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string& name, float value1, float value2) const;
};

#endif /* ShaderConstructor_h */
