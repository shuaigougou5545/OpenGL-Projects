#ifndef ShaderConstructor_h
#define ShaderConstructor_h

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <vector>


class ShaderConstructor{
private:
    std::string vsPath, fsPath, gsPath;
    
public:
    unsigned int ID; // shader program ID
    
    ShaderConstructor(const std::string vs_path, const std::string fs_path, const std::string vsMacroString = "", const std::string fsMacroString = "", const std::string gs_path = "");
    ~ShaderConstructor();
    void use();
    
    // set-uniform functions
    void setBool(const std::string &name, bool value, GLint LocationOffset = 0) const;
    void setInt(const std::string &name, int value, GLint LocationOffset = 0) const;
    void setFloat(const std::string &name, float value, GLint LocationOffset = 0) const;
    
    void setVec2(const std::string &name, float value1, float value2, GLint LocationOffset = 0) const;
    void setVec3(const std::string &name, float value1, float value2, float value3, GLint LocationOffset = 0) const;
    void setVec4(const std::string &name, float value1, float value2, float value3, float value4, GLint LocationOffset = 0) const;
    
    void setVec2(const std::string &name, const float *value, GLint LocationOffset = 0) const;
    void setVec3(const std::string &name, const float *value, GLint LocationOffset = 0) const;
    void setVec4(const std::string &name, const float *value, GLint LocationOffset = 0) const;
    
    void setMat2(const std::string &name, const float *value, GLint LocationOffset = 0) const;
    void setMat3(const std::string &name, const float *value, GLint LocationOffset = 0) const;
    void setMat4(const std::string &name, const float *value, GLint LocationOffset = 0) const;
    
    void setUniformBlock(const std::string &name, const int idx) const;
    
    GLint getLocation(const std::string &name) const;
    GLint getBlockIndex(const std::string &name) const;
};

#endif /* ShaderConstructor_h */
