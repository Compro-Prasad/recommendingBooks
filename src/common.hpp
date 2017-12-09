#include <vector>
#include <ostream>

template <typename T>
std::ostream& operator<<(std::ostream &out, std::vector<T> v) {
    for (auto &i : v)
        out << i;
    return out;
}
