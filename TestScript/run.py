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
fnull = "&> /dev/null"
all_utilities_path = "./run.master"
output_file = open(output_filename, "w")

# test cases path
test_list = []
for i in range(test_num):
	test_list.append("%s%s%d" % (test_path, test_prefix, i))


with all_utilities_file as open(all_utilities_path, "r"):
	utilities = all_utilities_file.readlines()
	for item in utilities:
		type = item.split(" ", 1)[0]
		cmd = item.split(" ", 1)[1]

		if type == "run.stdin":
			run_stdin(cmd, output_file, test_list, fnull, timeout)

		elif type == "run.file":
			run_file(cmd, output_file, test_list, fnull, timeout)

		elif type == "run.cp":
			run_cp(cmd, output_file, test_list, fnull, timeout)

		elif type == "run.double":
			run_double(cmd, output_file, test_list, fnull, timeout)

	all_utilities_file.close()
	output_file.close()




def run_file(cmd, output, test_list, fnull, timeout):
	for test_case in test_list:
		try:
			retcode = subprocess.call(["%s" % cmd, "%s" % test_case], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
			if retcode < 0:
				output.write("%s: %s error: %d\n" % (cmd, test_case, retcode))
		except:
			output.write("%s: %s hang\n" % (cmd, test_case))

	
def run_stdin(cmd, output, test_list, fnull, timeout):
	for test_case in test_list:
		try:
			retcode = subprocess.call(["%s" % cmd, "< %s" % test_case], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
			if retcode < 0:
				output.write("%s: %s error: %d\n" % (cmd, test_case, retcode))
		except:
			output.write("%s: %s hang\n" % (cmd, test_case))

def run_double(cmd, output, test_list, fnull, timeout):
	for i in range(len(test_list)):
		test_case1 = random.choice(test_list)
		test_case2 = random.choice(test_list)
		try:
			retcode = subprocess.call(["%s" % cmd, "%s %s" % (test_case2, test_case2)], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
			if retcode < 0:
				output.write("%s: %s %s error: %d\n" % (cmd, test_case1, test_case2, retcode))
		except:
			output.write("%s: %s %s hang\n" % (cmd, test_case1, test_case2))

