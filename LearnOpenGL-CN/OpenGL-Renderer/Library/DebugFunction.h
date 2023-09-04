#ifndef DebugFunction_h
#define DebugFunction_h

#include <glad/glad.h>
#include <iostream>


GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::cout << errorCode << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#endif /* DebugFunction_h */
