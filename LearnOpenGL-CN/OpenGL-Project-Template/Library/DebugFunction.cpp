#include "DebugFunction.h"


GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::cout << errorCode << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
