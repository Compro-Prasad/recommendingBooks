#!/usr/bin/python3

from bs4 import BeautifulSoup
import sys

soup = BeautifulSoup(open(sys.argv[1]), 'html5lib')

multipleGenres = soup.find_all('span', {'class': '_G0d'})

if not multipleGenres:
    singleGenre = soup.find_all('span', {'class': '_m3b'})
    if not singleGenre:
        print("NULL")
    else:
        print(singleGenre[0].text)
else:
    value = ""
    for i in multipleGenres:
        value += i.get_text()
    print(value)
