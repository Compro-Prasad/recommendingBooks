#include "./csv.hpp"
#include "./common.hpp"

bool exists(const char *file) {
    ifstream f(file);
    return f.is_open();
}

int main() {
    try {
        const char *newBooksCSV = "BX-Books-new.csv";
        const char *usersCSV = "BX-Users.csv";
        const char *ratingsCSV = "BX-Book-Ratings.csv";
        const char *genCSV = "BX-final.csv";
        if (exists(newBooksCSV) == false) {
            const char *oldBooksCSV = "BX-Books.csv";
            CSV books(oldBooksCSV, ';', '\\');
            CSV newbooks(newBooksCSV, ';', '\\', -1, 'w');
            try {
                cout << "Selectively copying contents from '"
                     << oldBooksCSV << "' to '" << newBooksCSV << "'...";
                cout.flush();
                while (true) {
                    vector<string> row = std::move(books.getNextRow());
                    if (row.size() == 0) continue;
                    vector<string> test(row.begin(), row.end() - 3);
                    newbooks.write(row.begin(), row.end() - 3);
                }
            } catch (const string &s) {
                if (s == "File is over")
                    cout << endl << "Created '" << newBooksCSV << "'" << endl;
                else {
                    cout << endl << s << endl;
                    exit(1);
                }
            }
        }
        CSV books(newBooksCSV, ';', '\\', 0);
        CSV users(usersCSV, ';', '\\', 0);
        CSV ratings(ratingsCSV, ';', '\\');
        cout << "Indexing users CSV...";
        cout.flush();
        users.indexAll();
        cout << "Done"<< endl;
        cout << "Indexing books CSV...";
        cout.flush();
        books.indexAll();
        cout << "Done" << endl;
        int k = 0;

        CSV generated(genCSV, ';', '\\', -1, 'w'); // Main CSV

        try {
            cout << "Creating '" << genCSV << "'...";
            cout.flush();
            while (true) {
                vector<string> rat = std::move(ratings.getNextRow());
                vector<string> usr = std::move(users.getRow(rat[0]));
                vector<string> bok = std::move(books.getRow(rat[1]));
                if (usr.size() == 0 ||
                    bok.size() == 0 ||
                    rat.size() == 0) {
                    k++;
                    continue;
                }
                vector<string> gen = {
                    bok[0], bok[1], bok[2], bok[3], bok[4],
                    rat[2],
                    usr[0], usr[1], usr[2]
                };

                generated.write(gen.begin(), gen.end());
            }
        } catch (const string &s) {
            if (s == "File is over")
                cout << endl << "Created '" << genCSV << "'" << endl;
            else {
                cout << endl << s << endl;
                exit(1);
            }
        }
        cout << "K: " << k << endl;
    } catch (const string &s) {
        cout << s << endl;
    }
    return 0;
}
