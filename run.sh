#!/bin/sh

ProjectDir=$(dirname $(readlink -v -f $(basename $0)))

cd $ProjectDir

mv data/BX-CSV-Dump.zip $ProjectDir

unzip $ProjectDir/BX-CSV-Dump.zip

rm $ProjectDir/BX-CSV-Dump.zip

dos2unix *.csv

make

./combineCSV
