#ifndef LoadTexture_h
#define LoadTexture_h

#include <vector>
#include <string>


unsigned int loadTexture(char const * path);
unsigned int loadCubemap(std::vector<std::string> faces);


#endif /* LoadTexture_h */
