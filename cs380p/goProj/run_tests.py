from subprocess import check_output
import re
import sys

times = []
time = 0.0

cmd = "./BST -input ./input/fine.txt -hash-workers {}".format(int(sys.argv[1]))
checkStr = "hashTime: (.*)"

if len(sys.argv) > 3:
    cmd = "./BST -input ./input/fine.txt -hash-workers {} -data-workers {}".format(int(sys.argv[1]), int(sys.argv[2]))
    checkStr = "hashGroupTime: (.*)"

if len(sys.argv) > 4:
    cmd = "./BST -input ./input/fine.txt -hash-workers {} -data-workers {} -comp-workers {}".\
          format(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]))
    checkStr = "compareTreeTime: (.*)"

if len(sys.argv) > 5:
    cmd = "./BST -input ./input/fine.txt -hash-workers {} -data-workers {} -comp-workers {} -comp-workers-flag {}".\
          format(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]))
    checkStr = "compareTreeTime: (.*)"

print(cmd)

for i in range(0,int(sys.argv[-1])):
   out = check_output(cmd, shell=True).decode("ascii")
   m = re.search(checkStr, out)
   if m is not None:
       times.append(float(m.group(1)))
       time += float(m.group(1))

print (time/int(sys.argv[-1]))
