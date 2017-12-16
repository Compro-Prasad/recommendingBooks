#ifndef GET_GENRE_HPP
#define GET_GENRE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

std::string trim(const std::string &str) {
    std::string retVal = "";
    auto begin = str.find_first_not_of(" \t");
    const auto end = str.find_last_not_of(" \t");
    bool f = 0;
    while (begin <= end) {
        if (str[begin] == ' ' || str[begin] == '\t') {
            f = 1;
            begin++;
            continue;
        } else if (f == 1) {
            f = 0;
            retVal += ' ';
        }
        retVal += str[begin++];
    }
    return retVal;
}

static std::string getChar(char c) {
    switch(c) {
    case ' ': return "+";
    case '/': return "%2F";
    case ',': return "%2C";
    case '+': return "%2B";
    case '=': return "%3D";
    case '#': return "%23";
    case '$': return "%24";
    case '%': return "%25";
    case '&': return "%26";
    case '@': return "%40";
    case ';': return "%3B";
    case ':': return "%3A";
    case '?': return "%3F";
    case '\'': return "%27";
    default: std::string x = {c, 0};
        return x;
    }
}

static std::string getSearchQueryURL(const std::string &str) {
    std::string retVal = "";
    for (auto &i : str) {
        retVal += getChar(i);
    }
    return retVal;
}

const char *getCstr(const std::string &str) {
    char *x = new char[str.length() + 1];
    size_t j = 0;
    for (auto &i : str) {
        if (i != '\0')
            x[j++] = i;
    }
    x[j] = '\0';
    return x;
}

static bool downloadPage(const std::string &str) {
    char *cmd = new char[4096];
    const char * const query = getCstr(str);
    snprintf(cmd, 4096,
             "wget --header=\"User-Agent: Mozilla/5.0 Gecko/2010 Firefox/5\""
             " --header=\"Accept: text/html,application/xhtml+xml,"
             "application/xml;q=0.9,*/*;q=0.8\""
             " --header=\"Accept-Language: en-us,en;q=0.5\""
             " --header=\"Accept-Encoding: gzip, deflate\""
             " --header=\"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\""
             " --header=\"Cookie: lang=us;"
             " reg=1787081http%%3A%%2F%%2Fwww.site.com%%2Fdc%%2F517870b8cc7\""
             " 'https://encrypted.google.com/search?hl=en"
             "&ei=Gg4sWq_bJMn5vATgr4DwBA"
             "&q=%s+book+genre' -O test.html > /dev/null 2>&1", query);
    delete[]query;
    int returnValue = system(cmd);
    delete[]cmd;
    return !returnValue;
}

static std::string parseHTML(const char *cmd) {
    FILE *stream = popen(cmd, "r");

    std::string genre = "";
    if (stream) {
        while (!feof(stream)) {
            char buff[256];
            const auto size = sizeof(buff);
            if (fgets(buff, size - 1, stream) != NULL)
                genre += buff;
        }
    } else {
        std::cerr << "parseHTML Error: Failed to run command" << std::endl;
        exit(1);
    }
    return genre.substr(genre.find_first_not_of("\n\r \t"),
                        genre.find_last_not_of("\n\r \t") + 1);
}

std::string getGenre(const std::string &str) {
    std::string searchQuery = getSearchQueryURL(trim(str));
    std::cout << searchQuery;
    downloadPage(searchQuery);
    std::string genre = parseHTML("./src/parseHTML.py test.html");
    std::cout << " -> " << genre << std::endl;
    return genre;
}
/*
int main(int argc, char **argv) {
    using namespace std;
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <search query>" << endl;
        exit(1);
    }
    cout << getGenre(argv[1]) << endl;
    return 0;
}
*/
#endif // GET_GENRE_HPP
