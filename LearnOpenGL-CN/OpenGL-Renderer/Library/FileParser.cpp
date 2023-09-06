#include "FileParser.h"


FileParser::FileParser()
{
      
}

FileParser::~FileParser()
{
      
}

std::string FileParser::getExtension(const std::string& fileName)
{
    size_t pos = fileName.rfind('.');
    if(pos != std::string::npos)
    {
        std::string fileExtension = fileName.substr(pos + 1);
        return fileExtension;
    }
    return std::string();
}
