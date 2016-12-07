#!/bin/bash

NUM=19

while [ $NUM -lt 37 ]; do
    str="$NUM"
    let NUM=NUM+1
    directory="large_$str"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~Start LARGE $str~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    confFile="tests/1000-large-$str.conf"
    make clean > /dev/null
    make > /dev/null
    rm -rf $directory
    mkdir $directory
    ./job_simulation $confFile > $directory/stdout.log
    head -10 $directory/stdout.log
    tail -6 $directory/stdout.log
    mv *.dat $directory/.
    cp $confFile $directory/.
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~End LARGE $str~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
done
