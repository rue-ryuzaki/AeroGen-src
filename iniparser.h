#ifndef INIPARSER_H
#define INIPARSER_H

#include <map>
#include <string>

#include "functions.h"

class IniParser
{
public:
    explicit IniParser(const char * path);
    ~IniParser();
    
    std::string  getProperty(std::string key);
    
private:
    void    addProperty(std::string key, std::string value);

    std::map<std::string, std::string> property;
};

#endif /* INIPARSER_H */

