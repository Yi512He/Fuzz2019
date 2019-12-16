#!/bin/sh

# go to Xv6 directory
echo cd test_Xv6
cd test_Xv6

# start xv6, run testScript, and then shut down xv6
echo ../tester/run-xv6-command.exp CPUS=1 Makefile testScript
../tester/run-xv6-command.exp CPUS=1 Makefile testScript

# make clean
echo make clean
make clean

