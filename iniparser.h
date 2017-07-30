#ifndef INIPARSER_H
#define INIPARSER_H

#include <map>
#include <string>

#include "functions.h"

class IniParser
{
public:
    explicit IniParser(const char* path);
    
    std::string  getProperty(const std::string& key);
    
private:
    void    addProperty(const std::string& key, const std::string& value);

    std::map<std::string, std::string> property;
};

#endif // INIPARSER_H

