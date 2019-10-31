import os
import subprocess
import sys
import random

# can change
test_path = "./NewTest/"
test_prefix = "t"
test_num = 599
timeout = 300
arg_error_detect = 3
output_filename_dir = "./output_log"


# can't change
fnull = open(os.devnull, 'w')
all_utilities_path = "./run.master_debug"
# output_file = open(output_filename, "w")

# test cases path
test_list = []
for i in range(1, test_num+1):
  test_list.append("%s%s%d" % (test_path, test_prefix, i))



def run_file(item, output, test_list, fnull, timeout):
  type = item.split(" ", 1)[0]
  cmd = item.split(" ", 1)[1]
  hang_count = 0
  for test_case in test_list:
    if hang_count >= arg_error_detect:
      break
    try:
      retcode = subprocess.call(["%s" % cmd, "%s" % test_case], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s hang\n" % (type, cmd, test_case))
    except(FileNotFoundError):
      output.write("%s %s not found\n" % (type, cmd))
      break
    else:
      hang_count = 0
      if retcode < 0:
        output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))
    # output.flush()

def run_cp(item, output, test_list, fnull, timeout):
  type = item.split(" ", 2)[0]
  file_tmp = item.split(" ", 2)[1]
  cmd = item.split(" ", 2)[2]
  hang_count = 0
  for test_case in test_list:
    if hang_count >= arg_error_detect:
      break
    try:
      subprocess.call(["cp", "%s" % test_case, "%s" % file_tmp])
      retcode = subprocess.call(["%s" % cmd, "%s" % file_tmp], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s hang\n" % (type, cmd, test_case))
    except(FileNotFoundError):
      output.write("%s %s not found\n" % (type, cmd))
      break
    else:
      hang_count = 0
      if retcode < 0:
        output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))
        subprocess.call(["rm", "%s" % file_tmp])
    # output.flush()
	
def run_stdin(item, output, test_list, fnull, timeout):
  type = item.split(" ", 1)[0]
  cmd = item.split(" ", 1)[1]
  hang_count = 0
  for test_case in test_list:
    if hang_count >= arg_error_detect:
      break
    try:
      retcode = subprocess.call(["%s" % cmd, "< %s" % test_case], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s hang\n" % (type, cmd, test_case))
    except(FileNotFoundError):
      output.write("%s %s not found\n" % (type, cmd))
      break
    else:
      hang_count = 0
      if retcode < 0:
        output.write("%s %s %s error: %d\n" % (type, cmd, test_case, retcode))
    # output.flush()

def run_double(item, output, test_list, fnull, timeout):
  type = item.split(" ", 1)[0]
  cmd = item.split(" ", 1)[1]
  hang_count = 0
  for i in range(len(test_list)):
    if hang_count >= arg_error_detect:
      break
    test_case1 = random.choice(test_list)
    test_case2 = random.choice(test_list)
    try:
      retcode = subprocess.call(["%s" % cmd, "%s %s" % (test_case1, test_case2)], stdout=fnull, stderr=subprocess.STDOUT, timeout=timeout)
    except(subprocess.TimeoutExpired):
      hang_count = hang_count + 1
      output.write("%s %s %s %s hang\n" % (type, cmd, test_case1, test_case2))
    except(FileNotFoundError):
      output.write("%s %s not found\n" % (type, cmd))
      break
    else:
      hang_count = 0
      if retcode < 0:
        output.write("%s %s %s %s error: %d\n" % (type, cmd, test_case1, test_case2, retcode))
    # output.flush()







# start
# create dir for log
if not os.path.exists(output_filename_dir):
  os.mkdir(output_filename_dir)

# open run.master and test every cmd
with open(all_utilities_path, "r") as all_utilities_file:
  utilities = all_utilities_file.readlines()
  for item in utilities:
    item = item.strip()
    type = item.split(" ", 1)[0]
    print(item)

    if type == "run.stdin":
      cmd = item.split(" ", 1)[1]
      file_name = os.path.join(output_filename_dir, "%s.%s" % (type, cmd))
      if os.path.exists(file_name):
        with open(file_name, "r") as f:
          if f.readlines[-1] != "finished\n":
            continue
      output_file = open(file_name, "w")
      output_file.write("start: %s\n" % item)
      run_stdin(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

    elif type == "run.file":
      cmd = item.split(" ", 1)[1]
      file_name = os.path.join(output_filename_dir, "%s.%s" % (type, cmd))
      if os.path.exists(file_name):
        with open(file_name, "r") as f:
          if f.readlines[-1] != "finished\n":
            continue
      output_file = open(file_name, "w")
      output_file.write("start: %s\n" % item)
      run_file(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

    elif type == "run.cp":
      cmd = item.split(" ", 2)[2]
      file_name = os.path.join(output_filename_dir, "%s.%s" % (type, cmd))
      if os.path.exists(file_name):
        with open(file_name, "r") as f:
          if f.readlines[-1] != "finished\n":
            continue
      output_file = open(file_name, "w")
      output_file.write("start: %s\n" % item)
      run_cp(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

    elif type == "run.double":
      cmd = item.split(" ", 1)[1]
      file_name = os.path.join(output_filename_dir, "%s.%s" % (type, cmd))
      if os.path.exists(file_name):
        with open(file_name, "r") as f:
          if f.readlines[-1] != "finished\n":
            continue
      output_file = open(file_name, "w")
      output_file.write("start: %s\n" % item)
      run_double(item, output_file, test_list, fnull, timeout)
      output_file.write("finished\n")
      output_file.close()

  all_utilities_file.close()
  fnull.close()






