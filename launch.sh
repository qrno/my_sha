#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <from> <to>"
    exit 1
fi

from=$1
to=$2

num_jobs=4
step=$(( ($to - $from + 1 + $num_jobs) / $num_jobs ))

calculate_range() {
    local subrange_start=$(($2 + $3 * $1))
    local subrange_end=$((subrange_start + $3))
    echo "$subrange_start $subrange_end"
}

log_dir="./logs"
mkdir -p $log_dir  # Create directory if it doesn't exist

log_folder="$log_dir/log_($from)_($to)"
mkdir -p $log_folder  # Create directory if it doesn't exist

for (( i=0; i<num_jobs; i++ )); do
    range=$(calculate_range $i $from $step)
    start=$(echo $range | cut -d ' ' -f 1)
    end=$(echo $range | cut -d ' ' -f 2)
    log_file="$log_folder/log_($start)_($end).txt"
    ./main $start $end >> $log_file 2>&1 &
    echo "Launched ./main $start $end with output redirected to $log_file"
done

wait

echo "All instances have finished running."
