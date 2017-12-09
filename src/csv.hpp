#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>


using namespace std;


/* CSV file is parsed and a vector<string> list is generated per row.  Currently
   there is no means to read a specific column.  For that purpose a ‘primary key
   column’ can be defined to get a specific row(getRow method) given the primary
   key value.

   If the primary key column has duplicate fields then the first one is only
   considered. The rest don’t get stored and can’t be accessed using getRow
   method. For this sole purpose it is ‘highly discourged’ to keep any duplicate
   field columns as a primary key */
class CSV {
    fstream file;               /* Contains the opened file metadata */
    string fileName;            /* The CSV filename in use */
    char separator;             /* The field separator */
    char escapeChar;            /* Character used to escape ‘"’ */
    int columnCount;            /* Number of columns in first non empty row.
                                 * Initialized on call to getNextrow */
    map<string, long> location; /* Map of (primKey -> file pos) */
    char fileMode;              /* Opened in read or write mode */
    bool isIndexed;             /* Check if all the columns are indexed */
    int primKeyCol;             /* Zero based primary key */
    long length;                /* File size */

public:
    explicit CSV(const string &fileName,
                 const char delimiter = ',',
                 const char escapeChar = '"',
                 const int primKey = -1,
                 const char RW = 'r') {
        if (RW == 'r')
            this->file.open(fileName, ios::in);
        else if (RW == 'w')
            this->file.open(fileName, ios::out | ios::in | ios::trunc);
        else
            throw string(fileName + " Error: Wrong RW flag");
        this->fileName = fileName;
        this->fileMode = RW;
        if (not file.is_open())
            throw string(this->fileName + " Error: File doesn't exist\n");
        this->columnCount = 0;
        this->separator = delimiter;
        this->escapeChar = escapeChar;
        this->isIndexed = false;
        this->primKeyCol = primKey;
        file.seekg(0, ios::end);
        this->length = file.tellg();
        file.seekg(0, ios::beg);
    }

    /* Write the contents of the vector into CSV file */
    void write(const vector<string>::iterator &begin,
               const vector<string>::iterator &end) {
        if (this->fileMode != 'w')
            throw string(this->fileName + " Error: File opened for reading");

        file << *begin;
        for (auto i = begin + 1; i < end; ++i) {
            file << this->separator << *i;
        }
        file << "\n";
    }

    /* Get a specific row only if a primary key column has been
       defined and all the rows have been indexed beforehand */
    vector<string> getRow(string k) {
        if (this->isIndexed == false)
            throw string(this->fileName +
                         " Call indexAll before calling getRow");
        if (this->fileMode != 'r')
            throw string(this->fileName +
                         " Error(getRow): File opened for writing");

        if (file.tellg() < 0) {
            file.close();
            file.open(fileName, ios::in);
        }
        if (this->location.count(k) == 0) {
            /* Key doesn’t exists */
            return vector<string>(0);
        }
        file.seekg(this->location[k], ios::beg);

        char c;
        bool strFlag = 0;
        vector<string> row(1);
        size_t currentColumn = 0;
        while (file.get(c) && (strFlag != 0 || c != '\n')) {
            if (c == '\r')
                continue;
            if (strFlag == 1 && c == this->escapeChar && file.get(c)) {
                if (c == '"')
                    continue;
                file.seekg(-1, ios::cur);
            }
            if (c == '"')
                strFlag = strFlag ? 0 : 1;
            if (c != this->separator) {
                row[currentColumn] += c;
            } else if (strFlag == 0) {
                row.push_back(string());
                currentColumn++;
            }
        }
        return row;
    }

    /* Index all records/rows if not indexed */
    void indexAll() {
        if (this->isIndexed == true)
            return;
        if (this->primKeyCol == -1)
            throw string(this->fileName +
                         " Error(indexAll): No primary key specified");
        // file.seekg(0, ios::beg);
        try {
            while (true) {
                this->getNextRow();
            }
        } catch (const string &s) {
            this->isIndexed = true;
            file.seekg(0, ios::beg);
        }
    }

    /* Get next row from the current position in the file */
    vector<string> getNextRow() {
        if (this->fileMode != 'r')
            throw string(this->fileName +
                         " Error(getNextrow): File opened for writing");
        char c;
        bool strFlag = 0;
        vector<string> row(1);
        int currentColumn = 0;
        long filePos = file.tellg();

        if (file.tellg() < 0 && file.tellg() >= this->length) {
            if (this->isIndexed == false)
                this->isIndexed = true;
            throw string("File is over");
        }
        while (file.get(c) && (strFlag != 0 || c != '\n')) {
            if (c == '\r')
                continue;
            if (strFlag == 1 && c == this->escapeChar && file.get(c)) {
                /* Got an escape character inside a string */
                if (c == '"') {
                    row[currentColumn] += this->escapeChar + c;
                    continue;
                }
                file.seekg(-1, ios::cur);
            }

            if (c == '"')
                /* Check for string: If strFlag == 0, string is starting, else
                   string is ending*/
                strFlag = strFlag ? 0 : 1;

            if (c != this->separator) {
                /* Write the character in the current index of vector */
                row[currentColumn] += c;
            } else if (strFlag == 0) {
                /* Got a separator and not inside a string */
                row.push_back(string());
                if (this->primKeyCol == currentColumn) {
                    /* If this was the primary key column, store the file
                       position */
                    this->location.insert(make_pair(row[currentColumn], filePos));
                }
                currentColumn++;
            }
        }
        if (currentColumn == 0) {
            if (this->isIndexed == false)
                this->isIndexed = true;
            throw string("File is over");
        }
        if (this->columnCount == 0) {
            this->columnCount = currentColumn;
        }
        else if (this->columnCount != currentColumn) {
            char format[256] = " Error: column counts mismatch\n"
                "Required: %lu, Got: %lu", c[256];
            sprintf(c, format, this->columnCount, currentColumn);
            throw string(this->fileName + c);
        }
        if (this->primKeyCol == currentColumn) {
            /* If the primary key was the last column which would not fall
               before a speparator(, or ;) */
            this->location.insert(make_pair(row[currentColumn], filePos));
        }

        return row;
    }

    ~CSV() {
        file.close();
    }
};
