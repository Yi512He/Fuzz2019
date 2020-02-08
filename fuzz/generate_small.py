import os, sys, re
import subprocess
import random
import time

fnull = open(os.devnull, 'w')
path = "./NewTest_small2"

if not os.path.exists(path):
  os.mkdir(path)

inc = 200
start = 0

# -0
for i in range(start, start+inc):
  print(i)
  if os.path.isfile(os.path.join(path, "t%d" % i)):
    continue
  time.sleep(1.1)
  n = random.randint(0, 1e3)
  subprocess.call(["fuzz", "%d" % n, "-0", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + inc

# -a
for i in range(start, start+inc):
  print(i)
  if os.path.isfile(os.path.join(path, "t%d" % i)):
    continue
  time.sleep(1.1)
  n = random.randint(0, 1e3)
  subprocess.call(["fuzz", "%d" % n, "-a", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + inc

# -p
for i in range(start, start+inc):
  print(i)
  if os.path.isfile(os.path.join(path, "t%d" % i)):
    continue
  time.sleep(1.1)
  n = random.randint(0, 1e3)
  subprocess.call(["fuzz", "%d" % n, "-p", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + inc

# -0 + -l
for i in range(start, start+inc):
  print(i)
  if os.path.isfile(os.path.join(path, "t%d" % i)):
    continue
  time.sleep(1.1)
  l = 50
  n = random.randint(0, 100)
  subprocess.call(["fuzz", "%d" % n, "-l %d" % l, "-0", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + inc

# -a + -l
for i in range(start, start+inc):
  print(i)
  if os.path.isfile(os.path.join(path, "t%d" % i)):
    continue
  time.sleep(1.1)
  l = 50
  n = random.randint(0, 100)
  subprocess.call(["fuzz", "%d" % n, "-l %d" % l, "-a", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + inc

# -p + -l
for i in range(start, start+inc):
  print(i)
  if os.path.isfile(os.path.join(path, "t%d" % i)):
    continue
  time.sleep(1.1)
  l = 50
  n = random.randint(0, 100)
  subprocess.call(["fuzz", "%d" % n, "-l %d" % l, "-p", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)

