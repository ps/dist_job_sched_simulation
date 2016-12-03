#!/bin/bash
ID=-1
NUM=$1
while [  $ID -lt $NUM ]; do
    python plot.py $ID
    let ID=ID+1 
done
