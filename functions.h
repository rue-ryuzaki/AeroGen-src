#ifndef AEROGEN_FUNCTIONS_H
#define AEROGEN_FUNCTIONS_H

#include <algorithm>
#include <cstring>
#include <vector>
#include <string>

inline std::vector<std::string> split(const std::string& str, char sep)
{
    std::vector<std::string> result;
    std::string value;
    for (const char& c : str) {
        if (c == sep) {
            result.push_back(value);
            value.clear();
        } else {
            value += c;
        }
    }
    result.push_back(value);
    return result;
}

// trim from start
inline std::string& ltrim(std::string& s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(std::ptr_fun<int, int>(isspace))));
    return s;
}

// trim from end
inline std::string& rtrim(std::string& s)
{
    s.erase(find_if(s.rbegin(), s.rend(), not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string& trim(std::string& s)
{
    return ltrim(rtrim(s));
}

inline std::string& replace(std::string& s, char olds, const std::string& news)
{
    size_t pos = s.find(olds); // find first space
    while (pos != std::string::npos) {
        s.replace(pos, 1, news);
        pos = s.find(olds, pos);
    }
    return s;
}

inline std::string dtos(double value, int digits, bool removeZeros = false)
{
    std::string res;
    std::string val = std::to_string(value);
    int32_t d = 0;
    bool dig = false;
    for (const char& c : val) {
        if (c == ',' || c == '.') {
            if (!dig) {
                res += c;
                dig = true;
            }
            continue;
        }
        if (d < digits) {
            res += c;
            if (dig) {
                ++d;
            }
        }
    }
    
    if (removeZeros) {
        while (!res.empty() && res.back() == '0') {
            res.pop_back();
        }
    }
    
    if (res.back() == '.' || res.back() == ',') {
        res += "0";
    }
    return res;
}

inline void reverse(char s[])
{
    int32_t size = int32_t(strlen(s));
    for (int32_t i = 0, j = size - 1; i < j; ++i, --j) {
        s[i] ^= s[j];
        s[j] ^= s[i];
        s[i] ^= s[j];
    }
}

inline void itoa(int n, char s[])
{
    int32_t sign = n;

    if (n < 0) {
        n = -n;
    }
    int32_t i = 0;
    do {
        s[i++] = char(n % 10 + '0');
    } while ((n /= 10) > 0);

    if (sign < 0) {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
}

#endif // AEROGEN_FUNCTIONS_H
