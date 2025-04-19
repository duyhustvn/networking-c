#!/bin/bash

ITERATIONS=5
PROGRAM=../build/sniff_packet

run_benchmark() {
    local method=$1
    local iteration=$2
    local output_file="result_${method}_${iteration}.txt"

    # start program 
    sudo ${PROGRAM} ${method} > ${output_file} &
    SNIFFER_PID=$!

    # Wait for program to initialize
    sleep 2

    # Replay traffic 
    sudo tcprelay --mbps=1000 -i veth0 benchmark.pcap

    # Wait for processing to complete and collect results
    wait ${SNIFFER_PID}

    # Extract metrics from output file 
    local pps=$(grep "Throughput:" $output_file | awk '{print $2}')
    local mbps=$(grep "Bandwidth:" $output_file | awk '{print $2}') 

    echo "$method,$iteration,$pps,$mbps" >> benchmark_results.csv
}

# Header 
echo "method,iteration,pps,mbps" >> benchmark_results.csv

# Run benchmark
for i in $(seq 1 $ITERATIONS); do 
    echo "Running the iteration $i"
    
    echo "Testing the ring buffer method"
    run_benchmark "ring" $i

    echo "Testing the poll method"
    run_benchmark "poll" $i
done


# Generate summary statistics
echo "Calculating statistics..."
python3 - <<EOF
import pandas as pd
df = pd.read_csv('benchmark_results.csv')
summary = df.groupby('method').agg({
    'packets_per_second': ['mean', 'std', 'min', 'max'],
    'mbps': ['mean', 'std', 'min', 'max']
})
summary.to_csv('benchmark_summary.csv')
print("\nResults Summary:")
print(summary)
EOF
