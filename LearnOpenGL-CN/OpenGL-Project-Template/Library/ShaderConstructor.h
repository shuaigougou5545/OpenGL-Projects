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
    
    void setVec2(const std::string &name, float value1, float value2) const;
    void setVec3(const std::string &name, float value1, float value2, float value3) const;
    void setVec4(const std::string &name, float value1, float value2, float value3, float value4) const;
    
    void setVec2(const std::string &name, const float *value) const;
    void setVec3(const std::string &name, const float *value) const;
    void setVec4(const std::string &name, const float *value) const;
    
    void setMat2(const std::string &name, const float *value) const;
    void setMat3(const std::string &name, const float *value) const;
    void setMat4(const std::string &name, const float *value) const;
};

#endif /* ShaderConstructor_h */
