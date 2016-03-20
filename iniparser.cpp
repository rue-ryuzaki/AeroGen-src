#include "iniparser.h"

IniParser::IniParser(const char * path) {
    ifstream in;
    in.open(path);
    string s = "";
    int line = 0;
    do {
        ++line;
        getline(in, s);
        if (trim(s) == "") {
            continue;
        }
        vector<string> vs = split(s, '=');
        if (vs.size() != 2) {
            cout << "Parse error at line " << line << " in '" << path << "'. Expect 'key = value'\n";
            break;
        }
        string key = trim(vs[0]);
        string value = trim(vs[1]);
        if (key == "" || value == "") {
            cout << "Parse error at line " << line << " in '" << path << "'. Empty key or value\n";
            break;
        } else {
            addProperty(key, value);
        }
    } while (!in.eof());
    in.close();
}

IniParser::~IniParser() {
}

string IniParser::getProperty(string key) {
    if (property.find(key) != property.end()) {
        return property.find(key)->second;
    }
    cerr << "Error to get property: " << key << endl;
    return "";
}

void IniParser::addProperty(string key, string value) {
    try {
        property.insert(pair<string, string>(key, value));
    } catch (...) {
        cerr << "Error to add property: " << key << " - " << value << endl;
    }
}
