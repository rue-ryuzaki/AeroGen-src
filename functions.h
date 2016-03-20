#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <string>
#include <string.h>
#include <bits/stl_algo.h>

using namespace std;

static vector<string> split(string &s, char c) {
    vector<string> result;
    string value = "";
    for (int i = 0; i < s.size(); ++i) {
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
static inline string &ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

// trim from end
static inline string &rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline string &trim(string &s) {
    return ltrim(rtrim(s));
}

static inline string &replace(string &s, char olds, string news) {
    int pos = s.find(olds); // find first space
    while (pos != string::npos) {
        s.replace(pos, 1, news);
        pos = s.find(olds, pos);
    }
    return s;
}

static string dtos(double value, int digits, bool removeZeros = false) {
    string res = "";
    string val = std::to_string(value);
    int d = 0;
    bool dig = false;
    for (int i = 0; i < val.size(); ++i) {
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
static void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; ++i, --j) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

static void itoa(int n, char s[]) {
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
