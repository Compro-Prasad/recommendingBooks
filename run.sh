#!/bin/sh

ScriptName="$(basename '$0')"
FullPathToScript="$(readlink -v -f '$ScriptName')"
ProjectDir="$(dirname '$FullPathToScript')"

unzip $ProjectDir/data/BX-CSV-Dump.zip -d $ProjectDir

dos2unix $ProjectDir/*.csv   # CRLF to LF

make -C "$ProjectDir"

./combineCSV

make -C "$ProjectDir" clean
