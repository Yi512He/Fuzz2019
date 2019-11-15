import os
import subprocess
import sys
import random

############
# options
# where are the test cases
test_dir = "./NewTest_debug/"

# the script will test all the files starting with a specified test_prefix
test_prefix = "t"

# if the cmd does not finish in timeout seconds, the test result will be considered as a hang
timeout = 20

# if there are hang_num successive hangs, no need to continue the testing on the current cmd
# check the way you use the cmd, or increase the timeout and retest, or consider the results as hangs
hang_num = 3

# the script will test each cmd in run.master on the test cases in test_dir
all_utilities_path = "./run.master_debug"

# the result will be saved in output_dir, each cmd corresponds to a result file 
output_dir = "./output_log_debug"

# the script will combine result files into single file named combine_filename
combine_filename = "all"
############


# redirect the output to /dev/null, otherwise the shell will be overwhelmed by outputs
fnull = open(os.devnull, 'w')


# get path of all test cases
test_list = []
test_list = os.listdir(test_dir)
test_list = [file for file in test_list if file.startswith(test_prefix)]
test_list = ["%s%s" % (test_dir, file) for file in test_list]

def run_file(item, output, test_list, fnull, timeout):
  type = item.split(" ", 1)[0]
  cmd = item.split(" ", 1)[1]
  hang_count = 0
  retcode = 0
  for test_case in test_list:
    print(test_case)
    if hang_count >= hang_num:
      break
    try:
      retcode = subprocess.call("%s %s" % (cmd, test_case), shell=True, stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s hang\n" % (type, cmd, test_case))
    except(FileNotFoundError):
      output.write("%s %s not found\n" % (type, cmd))
      break
    else:
      hang_count = 0
      if retcode == 139:
        output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))
    # output.flush()

def run_cp(item, output, test_list, fnull, timeout):
  type = item.split(" ", 2)[0]
  file_tmp = item.split(" ", 2)[1]
  cmd = item.split(" ", 2)[2]
  hang_count = 0
  retcode = 0
  for test_case in test_list:
    print(test_case)
    if hang_count >= hang_num:
      break
    try:
      subprocess.call("cp %s %s" % (test_case, file_tmp), shell=True)
      retcode = subprocess.call("%s %s" % (cmd, file_tmp), shell=True, stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s hang\n" % (type, cmd, test_case))
    #except(FileNotFoundError):
    #  output.write("%s %s not found\n" % (type, cmd))
    #  break
    else:
      hang_count = 0
      if retcode == 139:
        output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))
        subprocess.call("rm %s" % file_tmp, shell=True)
    # output.flush()
	
def run_stdin(item, output, test_list, fnull, timeout):
  type = item.split(" ", 1)[0]
  cmd = item.split(" ", 1)[1]
  hang_count = 0
  retcode = 0
  for test_case in test_list:
    print(test_case)
    if hang_count >= hang_num:
      break
    try:
      retcode = subprocess.call("%s < %s" % (cmd, test_case), shell=True, stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s hang\n" % (type, cmd, test_case))
    except(FileNotFoundError):
      output.write("%s %s not found\n" % (type, cmd))
      break
    else:
      hang_count = 0
      if retcode == 139:
        output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))
    # output.flush()

def run_double(item, output, test_list, fnull, timeout):
  type = item.split(" ", 1)[0]
  cmd = item.split(" ", 1)[1]
  hang_count = 0
  retcode = 0
  for i in range(len(test_list)):
    if hang_count >= hang_num:
      break
    test_case1 = random.choice(test_list)
    test_case2 = random.choice(test_list)
    try:
      retcode = subprocess.call("%s %s %s" % (cmd, test_case1, test_case2), shell=True, stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s %s hang\n" % (type, cmd, test_case1, test_case2))
    except(FileNotFoundError):
      output.write("%s %s not found\n" % (type, cmd))
      break
    else:
      hang_count = 0
      if retcode == 139:
        output.write("%s %s %s %s error: %d\n" % (type, cmd, test_case1, test_case2, retcode))
    # output.flush()







# start
# create dir for log
if not os.path.exists(output_dir):
  os.mkdir(output_dir)

# open run.master and test every cmd
with open(all_utilities_path, "r") as all_utilities_file:
  utilities = all_utilities_file.readlines()
  for item in utilities:
    item = item.strip()
    type = item.split(" ", 1)[0]
    print(item)

    if type == "run.stdin":
      cmd = item.split(" ", 1)[1]
      #print("h1")
      file_name = os.path.join(output_dir, "%s.%s" % (type, cmd.replace("/", "-")))
      if os.path.exists(file_name) and os.stat(file_name).st_size != 0:
        with open(file_name, "r") as f:
          if f.readlines()[-1] == "finished\n":
            continue
      output_file = open(file_name, "w")
      #print("h2")
      output_file.write("start: %s\n" % item)
      run_stdin(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

    elif type == "run.file":
      cmd = item.split(" ", 1)[1]
      file_name = os.path.join(output_dir, "%s.%s" % (type, cmd.replace("/", "-")))
      if os.path.exists(file_name) and os.stat(file_name).st_size != 0:
        with open(file_name, "r") as f:
          if f.readlines()[-1] == "finished\n":
            continue
      output_file = open(file_name, "w")
      output_file.write("start: %s\n" % item)
      run_file(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

    elif type == "run.cp":
      cmd = item.split(" ", 2)[2]
      file_name = os.path.join(output_dir, "%s.%s" % (type, cmd.replace("/", "-")))
      if os.path.exists(file_name) and os.stat(file_name).st_size != 0:
        with open(file_name, "r") as f:
          if f.readlines()[-1] == "finished\n":
            continue
      output_file = open(file_name, "w")
      output_file.write("start: %s\n" % item)
      run_cp(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

    elif type == "run.double":
      cmd = item.split(" ", 1)[1]
      file_name = os.path.join(output_dir, "%s.%s" % (type, cmd.replace("/", "-")))
      if os.path.exists(file_name) and os.stat(file_name).st_size != 0:
        with open(file_name, "r") as f:
          if f.readlines()[-1] == "finished\n":
            continue
      output_file = open(file_name, "w")
      output_file.write("start: %s\n" % item)
      run_double(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

  all_utilities_file.close()
  fnull.close()


# combine the result files
with open(combine_filename, "w") as file_to_write:
  dir = os.listdir(output_dir)
  dir.sort()
  for item in dir:
    with open(os.path.join(output_dir, item), "r") as file_to_read:
      file_to_write.write(file_to_read.read())



