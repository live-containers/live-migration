#!/usr/bin/python3
import os
import subprocess
import sys
import time

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def check_root():
    if os.geteuid() != 0:
        eprint("You need root permissions to run runc!")
        sys.exit(1)

def usage():
    txt = "Usage: {} --name [container id] ".format(sys.argv[0])
    txt += "[leave-running] [pre-copy|post-copy]"
    eprint(txt)
    sys.exit(1)

# TODO Functionalities to add
# -h for remote host migration
# implement proper pre and post copy
def parse_kwargs(arg_list):
    ret = {}
    if "--name" not in arg_list:
        usage()
    try:
        index = arg_list.index("--name")
        ret["container-name"] = arg_list[index + 1]
    except:
        usage()
    if "pre-copy" in arg_list:
        ret["pre-copy"] = 1
        ret["post-copy"] = 0
    if "post-copy" in arg_list:
        if "post-copy" not in ret: 
            ret["pre-copy"] = 0
            ret["post-copy"] = 1
        else:
            usage()
    return ret

def migration(**kwargs):
    # TODO check if container is there
    # Checkpoint
    cmd = "sudo runc checkpoint {}".format(kwargs["container-name"])
    start = time.time()
    p = subprocess.Popen(cmd, shell=True)
    p.wait()
    print("Checkpoint finished!")
    cp_time = time.time()
    cmd = "sudo runc restore -d {}-restored ".format(kwargs["container-name"])
    cmd += "&> /dev/null < /dev/null"
    p = subprocess.Popen(cmd, shell=True)
    p.wait()
    rt_time = time.time()
    print("Restore finished!")
    print("Time elapsed: \n\t- Checkpoint: {}\t- Restore: {}".format(cp_time,
          rt_time))

if __name__=="__main__":
    check_root()
    if len(sys.argv) < 2:
        usage()
    migration(**parse_kwargs(sys.argv))
