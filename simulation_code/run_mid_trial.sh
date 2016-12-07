#!/bin/bash

NUM=1

while [ $NUM -lt 37 ]; do
    str="$NUM"
    let NUM=NUM+1
    directory="mid_$str"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~Start MID $str~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    confFile="tests/1000-mid-$str.conf"
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
    echo "~~~~~~~~~~~~End MID $str~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
done
