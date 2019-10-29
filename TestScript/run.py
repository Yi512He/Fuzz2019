import os
import subprocess
import sys
import random

# can change
test_path = "./"
test_prefix = "t"
test_num = 12
timeout = 300
output_filename = "python_log"


# can't change
fnull = open(os.devnull, 'w')
all_utilities_path = "./run.master"
output_file = open(output_filename, "w")

# test cases path
test_list = []
for i in range(1, test_num+1):
	test_list.append("%s%s%d" % (test_path, test_prefix, i))



def run_file(item, output, test_list, fnull, timeout):
	type = item.split(" ", 1)[0]
	cmd = item.split(" ", 1)[1]
	for test_case in test_list:
		try:
			retcode = subprocess.call(["%s" % cmd, "%s" % test_case], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
		except:
			output.write("%s %s %s hang\n" % (type, cmd, test_case))
		else:
			if retcode < 0:
				output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))

def run_cp(item, output, test_list, fnull, timeout):
	type = item.split(" ", 2)[0]
	file_tmp = item.split(" ", 2)[1]
	cmd = item.split(" ", 2)[2]
	for test_case in test_list:
		try:
			subprocess.call(["cp", "%s" % test_case, "%s" % file_tmp])
			retcode = subprocess.call(["%s" % cmd, "%s" % file_tmp], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
		except:
			output.write("%s %s %s hang\n" % (type, cmd, test_case))
		else:
			if retcode < 0:
				output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))
		subprocess.call(["rm", "%s" % file_tmp])
	
def run_stdin(item, output, test_list, fnull, timeout):
	type = item.split(" ", 1)[0]
	cmd = item.split(" ", 1)[1]
	for test_case in test_list:
		try:
			retcode = subprocess.call(["%s" % cmd, "< %s" % test_case], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
		except:
			output.write("%s %s %s hang\n" % (type, cmd, test_case))
		else:
			if retcode < 0:
				output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))

def run_double(item, output, test_list, fnull, timeout):
	type = item.split(" ", 1)[0]
	cmd = item.split(" ", 1)[1]
	for i in range(len(test_list)):
		test_case1 = random.choice(test_list)
		test_case2 = random.choice(test_list)
		try:
			retcode = subprocess.call(["%s" % cmd, "%s %s" % (test_case1, test_case2)], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
		except:
			output.write("%s %s %s %s hang\n" % (type, cmd, test_case1, test_case2))
		else:
			if retcode < 0:
				output.write("%s %s %s %s error: %d\n" % (type, cmd, test_case1, test_case2, retcode))





with open(all_utilities_path, "r") as all_utilities_file:
	utilities = all_utilities_file.readlines()
	for item in utilities:
		item = item.strip()
		type = item.split(" ", 1)[0]
		print(item)

		if type == "run.stdin":
			run_stdin(item, output_file, test_list, fnull, timeout)

		elif type == "run.file":
			run_file(item, output_file, test_list, fnull, timeout)

		elif type == "run.cp":
			run_cp(item, output_file, test_list, fnull, timeout)

		elif type == "run.double":
			run_double(item, output_file, test_list, fnull, timeout)

	all_utilities_file.close()
	output_file.close()






