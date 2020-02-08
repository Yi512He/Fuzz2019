import os, re, sys
import hashlib
import random
import subprocess
from operator import itemgetter
import time

old_path = "./NewTest_normal"
new_path = "./normal_new"
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

sorted_dict_list = sorted(res_dict.items(), key=lambda item: item[1][0])

sorted_dict = {}
for item in sorted_dict_list:
	sorted_dict[item[0]] = item[1]

# exit()

# print(type(res_dict))
# exit()

for k, v in sorted_dict.items():
	if(len(v) > 1):
		print(v)

# exit()
for k, v in sorted_dict.items():
	if(len(v) > 1):
		print("start:", v)
		# print(v)

		for i in range(1, len(v)):
			time.sleep(2)
			name = v[i]
			name = name[1:]
			name = int(name)
			print(name)
			if(name in range(0, 100)):
				subprocess.call(["./fuzz", "-0", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(100, 200)):
				subprocess.call(["./fuzz", "-a", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(200, 300)):
				subprocess.call(["./fuzz", "-p", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(300, 400)):
				l = random.randint(1, 255)
				subprocess.call(["./fuzz", "-l %d" % l, "-0", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(400, 500)):
				l = random.randint(1, 255)
				subprocess.call(["./fuzz", "-l %d" % l, "-a", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(500, 600)):
				l = random.randint(1, 255)
				subprocess.call(["./fuzz", "-l %d" % l, "-p", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(600, 700)):
				l = random.randint(1, 255)
				n = random.randint(1, 5000)
				subprocess.call(["./fuzz", "%d" % n, "-l %d" % l, "-0", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(700, 800)):
				l = random.randint(1, 255)
				n = random.randint(1, 5000)
				subprocess.call(["./fuzz", "%d" % n, "-l %d" % l, "-a", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)
			elif(name in range(800, 900)):
				l = random.randint(1, 255)
				n = random.randint(1, 5000)
				subprocess.call(["./fuzz", "%d" % n, "-l %d" % l, "-p", "-o", os.path.join(new_path, "t%d" % name)], stdout=fnull, stderr=subprocess.STDOUT)

				
