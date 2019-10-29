import os, sys, re
import subprocess
import random

fnull = open(os.devnull, 'w')
path = "./NewTest"

if not os.path.exists(path):
	os.mkdir(path)


# -0
for i in range(13, 100):
	subprocess.call(["fuzz", "-0", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)

start = 100


# -a
for i in range(start, start+100):
	subprocess.call(["fuzz", "-a", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -p
for i in range(start, start+100):
	subprocess.call(["fuzz", "-p", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -0 + -l
for i in range(start, start+100):
	l = random.randint(1, 255)
	subprocess.call(["fuzz", "-l %d" % l, "-0", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -a + -l
for i in range(start, start+100):
	l = random.randint(1, 255)
	subprocess.call(["fuzz", "-l %d" % l, "-a", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -p + -l
for i in range(start, start+100):
	l = random.randint(1, 255)
	subprocess.call(["fuzz", "-l %d" % l, "-p", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)