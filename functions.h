#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <string>
#include <string.h>
#include <bits/stl_algo.h>

static std::vector<std::string> split(std::string &s, char c)
{
    std::vector<std::string> result;
    std::string value = "";
    for (int i = 0; i < int(s.size()); ++i) {
        if (s[i] == c) {
            result.push_back(value);
            value = "";
        } else {
            value += s[i];
        }
    }
    result.push_back(value);
    return result;
}

// trim from start
static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(std::ptr_fun<int, int>(isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s)
{
    s.erase(find_if(s.rbegin(), s.rend(), not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

static inline std::string &replace(std::string &s, char olds, std::string news)
{
    int pos = s.find(olds); // find first space
    while (pos != std::string::npos) {
        s.replace(pos, 1, news);
        pos = s.find(olds, pos);
    }
    return s;
}

static std::string dtos(double value, int digits, bool removeZeros = false)
{
    std::string res = "";
    std::string val = std::to_string(value);
    int d = 0;
    bool dig = false;
    for (int i = 0; i < int(val.size()); ++i) {
        if (val[i] == ',' || val[i] == '.') {
            if (!dig) {
                res += val[i];
                dig = true;
            }
            continue;
        }
        if (d < digits) {
            res += val[i];
            if (dig) {
                ++d;
            }
        }
    }
    
    if (removeZeros) {
        while (res.size() > 0 && res[res.size() - 1] == '0') {
            res.resize(res.size() - 1);
        }
    }
    
    if (res[res.size() - 1] == '.' || res[res.size() - 1] == ',') {
        res += "0";
    }
    return res;
}

/* reverse:  переворачиваем строку s на месте */
static void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; ++i, --j) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

static void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0) { /* записываем знак */
        n = -n; /* делаем n положительным числом */
    }
    i = 0;
    do { /* генерируем цифры в обратном порядке */
        s[i++] = n % 10 + '0'; /* берем следующую цифру */
    } while ((n /= 10) > 0); /* удаляем */
    if (sign < 0) {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
}

#endif /* FUNCTIONS_H */
