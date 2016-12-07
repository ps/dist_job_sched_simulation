#!/bin/bash
num_workers=( 5 10 20 30 )
#node_selection=( "SEQUENTIAL" "RANDOM" "SQS" ) # 1, 2,3
#job_dist=( "LINEAR" "CONSTANTK=10" "EXPONENTIAL" ) # 2 1 3
node_selection=( 1 2 3 )
job_dist=( 2 1 3 )


let c=1
for dist in "${job_dist[@]}"
do
    for node_sel in "${node_selection[@]}"
    do
        for workers in "${num_workers[@]}"
        do
            file_name="1000-small-$c.conf"
            rm $file_name
            touch $file_name
            echo "# Number of jobs to process" >> $file_name
            echo "1000" >> $file_name
            echo "" >> $file_name
            echo "# Job type (all small, all mid, all large, varied) = (1, 2, 3, 4)" >> $file_name
            echo "1" >> $file_name
            echo "" >> $file_name
            echo "# Number of worker nodes to use (max 5000)" >> $file_name
            echo "$workers" >> $file_name
            echo "" >> $file_name
            echo "# Worker job queue capacity" >> $file_name
            echo "50" >> $file_name
            echo "" >> $file_name
            echo "# Maximum number of jobs master thread is allowed to assign" >> $file_name
            echo "30" >> $file_name
            echo "" >> $file_name
            echo "# Worker node selection strategy (sequential, random, shortest queue) = (1, 2, 3)" >> $file_name
            echo "$node_sel" >> $file_name
            echo "" >> $file_name
            echo "# Job distribution strategy (constant, linear, exponential) = (1, 2, 3)" >> $file_name
            echo "$dist" >> $file_name
            echo "" >> $file_name
            echo "# If constant strategy, constant size" >> $file_name
            echo "10" >> $file_name

            echo $file_name
            echo $c, $workers, 50, 30, $node_sel, $dist
            let c=c+1
        done
    done
done
