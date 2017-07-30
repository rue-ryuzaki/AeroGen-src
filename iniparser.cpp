#include "iniparser.h"

#include <fstream>
#include <iostream>
#include <string>

IniParser::IniParser(const char* path)
{
    std::ifstream in;
    in.open(path);
    std::string s;
    int line = 0;
    do {
        ++line;
        getline(in, s);
        if (trim(s).empty()) {
            continue;
        }
        std::vector<std::string> vs = split(s, '=');
        if (vs.size() != 2) {
            std::cout << "Parse error at line " << line << " in '" << path << "'. Expect 'key = value'" << std::endl;
            break;
        }
        std::string key = trim(vs.at(0));
        std::string value = trim(vs.at(1));
        if (key.empty() || value.empty()) {
            std::cout << "Parse error at line " << line << " in '" << path << "'. Empty key or value" << std::endl;
            break;
        } else {
            addProperty(key, value);
        }
    } while (!in.eof());
    in.close();
}

std::string IniParser::getProperty(const std::string& key)
{
    auto it = property.find(key);
    if (it != property.end()) {
        return it->second;
    }
    std::cerr << "Error to get property: " << key << std::endl;
    return std::string();
}

void IniParser::addProperty(const std::string& key, const std::string& value)
{
    try {
        property.insert(std::pair<std::string, std::string>(key, value));
    } catch (...) {
        std::cerr << "Error to add property: " << key << " - " << value << std::endl;
    }
}
