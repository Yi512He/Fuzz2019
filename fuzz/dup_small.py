import os, re, sys
import hashlib
import random
import subprocess

old_path = "./NewTest_small"
new_path = "./small_new"
items = os.listdir(old_path)
fnull = open(os.devnull, 'w')

res_dict = {}

for item in items:
	with open(os.path.join(old_path, item),'rb') as f:
		s = f.read()
	res = hashlib.md5(s).hexdigest()
	if(res in res_dict.keys()):
		old_list = res_dict[res]
		old_list.append(item)
	else:
		res_dict[res] = [item]

for k, v in res_dict.items():
	if(len(v) > 1):
		v.sort()
		print(v)

# exit()
for k, v in res_dict.items():
	if(len(v) > 1):
		v.sort()

		for i in range(1, len(v)):
			name = v[i]
			name = name[1:]
			name = int(name)
			if(name in range(0, 200)):
				n = random.randint(0, 1e3)
				subprocess.call(["./fuzz", "%d" % n, "-0", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(200, 400)):
				n = random.randint(0, 1e3)
				subprocess.call(["./fuzz", "%d" % n, "-a", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(400, 600)):
				n = random.randint(0, 1e3)
				subprocess.call(["./fuzz", "%d" % n, "-p", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(600, 800)):
				l = 50
				n = random.randint(0, 100)
				subprocess.call(["./fuzz", "%d" % n, "-l %d" % l, "-0", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(800, 1000)):
				l = 50
				n = random.randint(0, 100)
				subprocess.call(["./fuzz", "%d" % n, "-l %d" % l, "-a", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(1000, 1200)):
				l = 50
				n = random.randint(0, 100)
				subprocess.call(["./fuzz", "%d" % n, "-l %d" % l, "-p", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)




