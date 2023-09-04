#ifndef DebugFunction_h
#define DebugFunction_h

#include <glad/glad.h>


GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif /* DebugFunction_h */
