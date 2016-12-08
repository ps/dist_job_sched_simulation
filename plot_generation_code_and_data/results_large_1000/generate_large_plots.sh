#!/bin/bash

NUM=1
num_workers=( 5 10 20 30 )
WORKERS_INDEX=0


while [  $NUM -lt 37 ]; do
	directory="large_$NUM"
	confFile="1000-large-$NUM.conf"
	echo "RUNNING: $confFile"
	cd $directory
	rm plot.py
	rm *.png
	cp ~/dist_job_sched_simulation/plot.py .

	let ID=-1
	let LIMIT=${num_workers[WORKERS_INDEX]}
	while [ $ID -lt $LIMIT ]; do
		python plot.py $ID $confFile
		let ID=ID+1
	done
	let WORKERS_INDEX=WORKERS_INDEX+1
	let BOUND=4
	if [ $WORKERS_INDEX -eq $BOUND ]; then
		let WORKERS_INDEX=0
	fi

    let NUM=NUM+1
    cd ..
done
