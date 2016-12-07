#!/bin/bash

NUM=1

while [ $NUM -lt 37 ]; do
    str="$NUM"
    let NUM=NUM+1
    directory="small_$str"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~Start SMALL $str~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    confFile="tests/1000-small-$str.conf"
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
    echo "~~~~~~~~~~~~End SMALL $str~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
done
