#include <unistd.h>
#include "./csv.hpp"
#include "./common.hpp"

int main(int argc, char **argv) {
    vector<size_t> columns;
    while (true) {
        char option = getopt(argc, argv, "c:f:");
        if (option == -1)
            break;
        switch (option) {
        case 'c':
            columns.clear();
            for (auto &i : split(optarg, ','))
                columns.push_back(toInt(i));
            break;
        case 'f':
            try {
                CSV csv(optarg, ';', '\\');
                if (columns.size() != 0) {
                    while (true) {
                        bool f = 0;
                        vector<string> row = move(csv.getNextRow());
                        for (size_t i = 0, k = 0; i < row.size(); ++i) {
                            if (i + 1 == columns[k]) {
                                if (f)
                                    cout << row[i] << ", ";
                                else
                                    cout << row[i];
                                f = 1;
                                ++k;
                            }
                        }
                        cout << endl;
                    }
                } else {
                    while (true) {
                        vector<string> row = move(csv.getNextRow());
                        cout << row << endl;
                    }
                }
            } catch (const string &s) {
                cout << s << endl;
            }
        }
    }
    return 0;
}
