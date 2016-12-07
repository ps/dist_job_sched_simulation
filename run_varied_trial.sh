#!/bin/bash

NUM=1

while [ $NUM -lt 37 ]; do
    str="$NUM"
    let NUM=NUM+1
    directory="varied_$str"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~Start VARIED $str~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    confFile="tests/1000-varied-$str.conf"
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
    echo "~~~~~~~~~~~~End VARIED $str~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
done
