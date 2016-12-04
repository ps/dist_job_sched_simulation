#!/bin/bash
ID=-1
NUM=$1
CONF=$2
while [  $ID -lt $NUM ]; do
    python plot.py $ID $CONF
    let ID=ID+1 
done
