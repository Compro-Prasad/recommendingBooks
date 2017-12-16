#include <iomanip>
#include <signal.h>
#include <unistd.h>
#include "./csv.hpp"
#include "./common.hpp"
#include "./getGenre.hpp"


const char *genCSV = "BX-final.csv";

/* Why global?

   So, that it can be accessed directly when a signal is caught and be
   closed properly so that we can continue with whatever information,
   we could crawl and/or generate.
*/
CSV generated(genCSV, ';', '\\', -1, 'w');// Main CSV that will be genrated

bool exists(const char *file) {
    ifstream f(file);
    return f.is_open();
}

void emergencyExit(int sig) {
    std::cerr << std::endl << "Recieved signal: " << sig << std::endl;
    std::cerr << "Closing file: " << genCSV << std::endl;
    generated.close();
    std::cerr << "Close successful" << std::endl;
    exit(0);
}

int main() {
    signal(SIGKILL, emergencyExit);
    signal(SIGABRT, emergencyExit);
    signal(SIGINT, emergencyExit);
    signal(SIGSEGV, emergencyExit);
    signal(SIGTERM, emergencyExit);
    try {
        const char *newBooksCSV = "BX-Books-new.csv";
        const char *usersCSV = "BX-Users.csv";
        const char *ratingsCSV = "BX-Book-Ratings.csv";
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
        map<string, string> bookGenre;
        cout << "Indexing users CSV...";
        cout.flush();
        users.indexAll();
        cout << "Done"<< endl;
        cout << "Indexing books CSV...";
        cout.flush();
        books.indexAll();
        cout << "Done" << endl;
        long rowsSkippedCount = 0;


        try {
            double prevProgress = 0, currentProgress = 0;
            cout << "Creating '" << genCSV << "'..." << endl << "0%";
            cout << fixed << showpoint;
            cout << setprecision(4);
            cout.flush();
            while (true) {
                vector<string> rat = std::move(ratings.getNextRow());
                vector<string> usr = std::move(users.getRow(rat[0]));
                vector<string> bok = std::move(books.getRow(rat[1]));
                if (usr.size() == 0 ||
                    bok.size() == 0 ||
                    rat.size() == 0) {
                    rowsSkippedCount++;
                    continue;
                }
                string genre;
                if (bookGenre.count(bok[0]) == 0) {
                    genre = getGenre(bok[1]); // bok[1] is the name of the book
                    bookGenre.insert(make_pair(bok[0], genre));
                } else {
                    genre = bookGenre[bok[0]];
                }
                vector<string> gen = {
                    bok[0], bok[1], bok[2], bok[3], bok[4], genre,
                    rat[2],
                    usr[0], usr[1], usr[2]
                };

                generated.write(gen.begin(), gen.end());

                currentProgress = double(ratings.currentFilePos()) /
                    double(ratings.size()) * 100;

                if (prevProgress + 0.000001  < currentProgress) {
                    prevProgress = currentProgress;
                    cout << "\r" << prevProgress << "%";
                    cout.flush();
                }
                sleep((rand() % 15) + 5);
            }
        } catch (const string &s) {
            if (s == "File is over")
                cout << endl << "Created '" << genCSV << "'" << endl;
            else {
                cout << endl << s << endl;
                exit(1);
            }
        }
        cout << "Skipped " << rowsSkippedCount
             << " records because of insuffient data" << endl;
    } catch (const string &s) {
        cout << s << endl;
    }
    return 0;
}
