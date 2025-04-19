#!/bin/bash

sudo tcpdump -i wlp1s0 -w benchmark.pcap -c 1000000 -W 1

# To check the benchmakr.pcap infor using command
# capinfos benchmark.pcap
