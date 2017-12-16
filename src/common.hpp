#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <ostream>
#include <string>

template <typename T>
std::ostream& operator<<(std::ostream &out, std::vector<T> v) {
    for (auto &i : v)
        out << i;
    return out;
}

std::vector<std::string> split(std::string s, char c) {
    using namespace std;
    string temp = "";
    vector<string> result;
    for (auto &i : s) {
        if (i != c) {
            temp.push_back(i);
        } else {
            result.push_back(move(temp));
        }
    }
    result.push_back(move(temp));
    return result;
}

int toInt(std::string s) {
    int result = 0;
    for (auto &i : s) {
        if (i != '\0') {
            if (isdigit(i))
                result = result * 10 + i - 48;
            else
                throw std::string("toInt Error: Not a digit\n");
        }
    }
    return result;
}

#endif
