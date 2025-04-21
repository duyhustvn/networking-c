#!/bin/bash

ITERATIONS=1
PROGRAM=./build/sniff_packet
MTU=1500 

# Create virtual interfaces pair for testing 
setup_virtual_interfaces() {
    echo "Create virtual interfaces"
    sudo ip link add veth0 type veth peer name veth1

    sudo ip link set veth0 mtu $MTU
    sudo ip link set veth1 mtu $MTU

    sudo ip link set veth0 up
    sudo ip link set veth1 up
}

cleanup_interfaces() {
    echo "cleaning up virtual interfaces" 
    sudo ip link del veth0 # veth1 will be automatically removed
}

run_benchmark() {
    local method=$1
    local iteration=$2
    local output_file="result_${method}_${iteration}.txt"

    # start program 
    sudo DEVICE_INTERFACE=veth1 ${PROGRAM} ${method} > ${output_file} &
    SNIFFER_PID=$!

    # Wait for program to initialize
    sleep 2

    # Replay traffic 
    sudo tcpreplay --mbps=1000 -i veth0 benchmark.pcap

    # Wait for processing to complete and collect results
    wait ${SNIFFER_PID}

    # Extract metrics from output file 
    local pps=$(grep "Throughput:" $output_file | awk '{print $2}')
    local mbps=$(grep "Bandwidth:" $output_file | awk '{print $2}') 

    echo "$method,$iteration,$pps,$mbps" >> benchmark_results.csv
}

# create virtual interface 
setup_virtual_interfaces

# Header 
echo "method,iteration,pps,mbps" >> benchmark_results.csv

# Run benchmark
for i in $(seq 1 $ITERATIONS); do 
    echo "Running the iteration $i"
    
    echo "Testing the ring buffer method"
    run_benchmark "ring" $i

    #echo "Testing the poll method"
    #run_benchmark "poll" $i
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

# cleanup
cleanup_interfaces
