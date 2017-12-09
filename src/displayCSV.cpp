#include "./csv.hpp"
#include "./common.hpp"

int main(int argc, char **argv) {
    try {
        CSV csv(argv[1], ';', '\\');
        while (true) {
            vector<string> row = std::move(csv.getNextRow());
            cout << row << endl;
        }
    } catch (const string &s) {
        cout << s << endl;
    }
    return 0;
}
