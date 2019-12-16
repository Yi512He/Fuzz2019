#!/bin/sh

# goto directpry ptyjig
echo cd ptyjig
cd ptyjig

# a sample testcase for testing vim
echo ./pty -d 0.05 vim < test_case_for_vim
./pty -d 0.05 vim < test_case_for_vim

# a sample testcase for testing htop
echo cat t1 ./end/end_htop > test_case_for_htop
cat t1 ./end/end_htop > test_case_for_htop
echo ./pty -d 0.05 htop < test_case_for_htop
./pty -d 0.05 htop < test_case_for_htop

