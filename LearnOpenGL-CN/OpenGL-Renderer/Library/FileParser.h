#ifndef FileParser_h
#define FileParser_h

#include <iostream>
#include <string>

class FileParser{
public:
    FileParser();
    ~FileParser();
    
    std::string getExtension(const std::string& fileName);
private:
    
};

#endif /* FileParser_h */
