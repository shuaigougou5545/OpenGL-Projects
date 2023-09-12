#include "ShaderConstructor.h"


ShaderConstructor::ShaderConstructor(const std::string vs_path, const std::string fs_path, const std::string vsMacroString, const std::string fsMacroString, const std::string gs_path) : vsPath(vs_path), fsPath(fs_path), gsPath(gs_path)
{
    bool isUseGS = (gsPath != "");
    
    std::string vsString, fsString, gsString;
    std::ifstream vsFile, fsFile, gsFile;
    // 保证文件可以抛出异常
    vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vsFile.open(vsPath.c_str());
        fsFile.open(fsPath.c_str());
        std::stringstream vsStream, fsStream;
        vsStream << vsFile.rdbuf();
        fsStream << fsFile.rdbuf();
        vsFile.close();
        fsFile.close();
        vsString = vsStream.str();
        fsString = fsStream.str();
        
        if(isUseGS)
        {
            gsFile.open(gsPath.c_str());
            std::stringstream gsStream;
            gsStream << gsFile.rdbuf();
            gsFile.close();
            gsString = gsStream.str();
        }
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR [Shader Constructor]: file not successfully read" << std::endl;
    }
    
    // insert macro definition - ps:glsl的第一行必须是#version...,所以宏只能放在version之后
    size_t pos;
    if((pos = vsString.find('\n')) != std::string::npos)
        vsString.insert(pos + 1, vsMacroString);
    if((pos = fsString.find('\n')) != std::string::npos)
        fsString.insert(pos + 1, fsMacroString);
    
    const char* vsCode = vsString.c_str();
    const char* fsCode = fsString.c_str();
    const char* gsCode = gsString.c_str();

    int success;
    char infoLog[512];
    
    unsigned int VS, FS, GS;
    
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
    
    if(isUseGS)
    {
        GS = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(GS, 1, &gsCode, NULL);
        glCompileShader(GS);
        glGetShaderiv(GS, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(GS, 512, NULL, infoLog);
            std::cout << "ERROR [Geometry Shader]: compilation failed" << std::endl;
        }
    }
    
    ID = glCreateProgram();
    
    glAttachShader(ID, VS);
    glAttachShader(ID, FS);
    if(isUseGS)
    {
        glAttachShader(ID, GS);
    }
        
    
    glLinkProgram(ID);
    
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR [Shader Program]: link failed" << std::endl;
    }
    
    glDeleteShader(VS);
    glDeleteShader(FS);
    if(isUseGS)
    {
        glDeleteShader(GS);
    }
        
}

ShaderConstructor::~ShaderConstructor()
{
    glDeleteProgram(ID);
}

void ShaderConstructor::use()
{
    glUseProgram(ID);
}

void ShaderConstructor::setBool(const std::string &name, bool value, GLint LocationOffset) const
{
    glUniform1i(getLocation(name) + LocationOffset, (int)value);
}

void ShaderConstructor::setInt(const std::string &name, int value, GLint LocationOffset) const
{
    GLint location = getLocation(name);
    glUniform1i(location + LocationOffset, value);
}

void ShaderConstructor::setFloat(const std::string &name, float value, GLint LocationOffset) const
{
    glUniform1f(getLocation(name) + LocationOffset, value);
}

void ShaderConstructor::setVec2(const std::string& name, float value1, float value2, GLint LocationOffset) const
{
    glUniform2f(getLocation(name) + LocationOffset, value1, value2);
}

void ShaderConstructor::setVec3(const std::string& name, float value1, float value2, float value3, GLint LocationOffset) const
{
    glUniform3f(getLocation(name) + LocationOffset, value1, value2, value3);
}

void ShaderConstructor::setVec4(const std::string& name, float value1, float value2, float value3, float value4, GLint LocationOffset) const
{
    glUniform4f(getLocation(name) + LocationOffset, value1, value2, value3, value4);
}

void ShaderConstructor::setVec2(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniform2f(getLocation(name) + LocationOffset, value[0], value[1]);
}

void ShaderConstructor::setVec3(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniform3f(getLocation(name) + LocationOffset, value[0], value[1], value[2]);
}

void ShaderConstructor::setVec4(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniform4f(getLocation(name) + LocationOffset, value[0], value[1], value[2], value[3]);
}

void ShaderConstructor::setMat2(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniformMatrix2fv(getLocation(name) + LocationOffset, 1, GL_FALSE, value);
}

void ShaderConstructor::setMat3(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniformMatrix3fv(getLocation(name) + LocationOffset, 1, GL_FALSE, value);
}

void ShaderConstructor::setMat4(const std::string &name, const float *value, GLint LocationOffset) const
{
    glUniformMatrix4fv(getLocation(name) + LocationOffset, 1, GL_FALSE, value);
}

void ShaderConstructor::setUniformBlock(const std::string &name, const int idx) const
{
    glUniformBlockBinding(ID, getBlockIndex(name), (GLint)idx);
}

GLint ShaderConstructor::getLocation(const std::string &name) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if(loc == -1)
        std::cout << "ERROR [Shader Uniform Location]: get uniform '" << name << "' location failed" << std::endl;
    return loc;
}

GLint ShaderConstructor::getBlockIndex(const std::string &name) const
{
    GLint idx = glGetUniformBlockIndex(ID, name.c_str());
    if(idx == -1)
        std::cout << "ERROR [Shader Uniform Block Index]: get uniform block '" << name << "' index failed" << std::endl;
    
    return idx;
}
