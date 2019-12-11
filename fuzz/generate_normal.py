import os, sys, re
import subprocess
import random

fnull = open(os.devnull, 'w')
path = "./NewTest_normal"

if not os.path.exists(path):
  os.mkdir(path)


# -0
for i in range(13, 100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  subprocess.call(["fuzz", "-0", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)

start = 100


# -a
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  subprocess.call(["fuzz", "-a", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -p
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  subprocess.call(["fuzz", "-p", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -0 + -l
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  l = random.randint(1, 255)
  subprocess.call(["fuzz", "-l %d" % l, "-0", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -a + -l
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  l = random.randint(1, 255)
  subprocess.call(["fuzz", "-l %d" % l, "-a", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -p + -l
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  l = random.randint(1, 255)
  subprocess.call(["fuzz", "-l %d" % l, "-p", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -0 + -l + 5000
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  l = random.randint(1, 255)
  n = random.randint(1, 5000)
  subprocess.call(["fuzz", "%d" % n, "-l %d" % l, "-0", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -a + -l + 5000
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  l = random.randint(1, 255)
  n = random.randint(1, 5000)
  subprocess.call(["fuzz", "%d" % n, "-l %d" % l, "-a", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)
start = start + 100

# -p + -l + 5000
for i in range(start, start+100):
  if os.path.isfile(os.path.join(path, "t%d" % i)):
  	continue
  l = random.randint(1, 255)
  n = random.randint(1, 5000)
  subprocess.call(["fuzz", "%d" % n, "-l %d" % l, "-p", "-o", os.path.join(path, "t%d" % i)], stdout=fnull, stderr=subprocess.STDOUT)