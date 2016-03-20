#ifndef INIPARSER_H
#define INIPARSER_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "functions.h"

using namespace std;

class IniParser {
public:
    IniParser(const char * path);
    ~IniParser();
    
    string getProperty(string key);
    
private:
    void addProperty(string key, string value);
    map<string, string> property;
};

#endif /* INIPARSER_H */

