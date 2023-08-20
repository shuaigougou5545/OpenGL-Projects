#include "ShaderConstructor.h"


ShaderConstructor::ShaderConstructor(const char* vsPath, const char* fsPath)
{
    std::string vsString, fsString;
    std::ifstream vsFile, fsFile;
    // 保证文件可以抛出异常
    vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vsFile.open(vsPath);
        fsFile.open(fsPath);
        std::stringstream vsStream, fsStream;
        vsStream << vsFile.rdbuf();
        fsStream << fsFile.rdbuf();
        vsFile.close();
        fsFile.close();
        vsString = vsStream.str();
        fsString = fsStream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR [Shader Constructor]: file not successfully read" << std::endl;
    }
    
    const char* vsCode = vsString.c_str();
    const char* fsCode = fsString.c_str();
    
    int success;
    char infoLog[512];
    
    unsigned int VS, FS;
    
    VS = glCreateShader(GL_VERTEX_SHADER);
    FS = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(VS, 1, &vsCode, NULL);
    glShaderSource(FS, 1, &fsCode, NULL);
    
    glCompileShader(VS);
    glCompileShader(FS);
    
    glGetShaderiv(VS, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(VS, 512, NULL, infoLog);
        std::cout << "ERROR [Vertex Shader]: compilation failed" << std::endl;
    }
    
    glGetShaderiv(FS, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(FS, 512, NULL, infoLog);
        std::cout << "ERROR [Fragment Shader]: compilation failed" << std::endl;
    }
    
    ID = glCreateProgram();
    
    glAttachShader(ID, VS);
    glAttachShader(ID, FS);
    
    glLinkProgram(ID);
    
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR [Shader Program]: link failed" << std::endl;
    }
    
    glDeleteShader(VS);
    glDeleteShader(FS);
}

void ShaderConstructor::use()
{
    glUseProgram(ID);
}

void ShaderConstructor::destory()
{
    glDeleteProgram(ID);
}

void ShaderConstructor::setBool(const std::string &name, bool value, GLint LocationOffset) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()) + LocationOffset, (int)value);
}

void ShaderConstructor::setInt(const std::string &name, int value, GLint LocationOffset) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value);
}

void ShaderConstructor::setFloat(const std::string &name, float value, GLint LocationOffset) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value);
}

void ShaderConstructor::setVec2(const std::string& name, float value1, float value2, GLint LocationOffset) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value1, value2);
}

void ShaderConstructor::setVec3(const std::string& name, float value1, float value2, float value3, GLint LocationOffset) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value1, value2, value3);
}

void ShaderConstructor::setVec4(const std::string& name, float value1, float value2, float value3, float value4, GLint LocationOffset) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value1, value2, value3, value4);
}

void ShaderConstructor::setVec2(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value[0], value[1]);
}

void ShaderConstructor::setVec3(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value[0], value[1], value[2]);
}

void ShaderConstructor::setVec4(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()) + LocationOffset, value[0], value[1], value[2], value[3]);
}

void ShaderConstructor::setMat2(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()) + LocationOffset, 1, GL_FALSE, value);
}

void ShaderConstructor::setMat3(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()) + LocationOffset, 1, GL_FALSE, value);
}

void ShaderConstructor::setMat4(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()) + LocationOffset, 1, GL_FALSE, value);
}
