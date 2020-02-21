#!/usr/bin/python3
import ast
import os
import subprocess
import shutil
import sys
import time

class CustomException(Exception):
    pass

def eprint(*args, **kwargs):
    pre = "Error!"
    print("{} {}".format(pre, *args), file=sys.stderr, **kwargs)

def check_root():
    if os.geteuid() != 0:
        eprint("You need root permissions to run runc!")
        sys.exit(1)

def usage():
    txt = "Usage: {} \n".format(sys.argv[0])
    txt += "\t--name <container id>\t\tname of the runC container\n"
    txt += "\t--image-path [dir]\t\tpath where to store the checkpoint files"
    txt += "for non-diskless migrations\n" #default /tmp/img-path
    #txt += "\t--hostname <host:port>
    #txt += "\t--container-path [dir|pwd]\t\tpath where the container image is\n"
    txt += "\t--diskless \t\tperform a diskless migration\n" 
    #txt += "[leave-running] [pre-copy|post-copy]\n"
    eprint(txt)
    sys.exit(1)

"""
=====
parse_kwargs returns an object with the following parameters.
=====

- container-name: String (Reuired)
        Container name 
- image-path: Path (Default: /tmp/criu/<container-name>)
        Directory to store checkpoint files if doing a non-diskless migration.
- container-path: Path
        Directory where the checkpointed container has the configuration files
- page-server: Tuple (Host, Port)
        Details of the page server to perform diskless/lazy migrations.
- diskless: Bool 
        If set, performs a diskless migration using /dev/shm
"""
def parse_kwargs(arg_list):
    ret = {}
    if "--name" not in arg_list:
        eprint("Missing mandatory argument --name!")
        usage()
    try:
        index = arg_list.index("--name")
        ret["container-name"] = arg_list[index + 1]
        tmp_out = subprocess.getoutput("sudo runc list --format")
        cont_list = ast.literal_eval(tmp_out)
        kwargs["container-path"] = [i for i in ast.literal_eval(a) 
                                    if i['id'] == name][0]["bundle"]
    except IndexError:
        eprint("{} is not a running container!".format(
              ret["container-name"]))
        sys.exit(1)
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
    if "--image-path" in arg_list:
        try:
            index = arg_list.index("--index-path")
            ret["image-path"] = arg_list[index + 1]
            if (ret["image-path"][-1] != "/"):
                ret["image-path"] += "/"
        except:
            eprint("In the container path argument.")
            usage()
    else:
        ret["image-path"] = "/tmp/criu/{}/".format(ret["container-name"])
        try:
            os.makedirs(ret["image-path"])
        except FileExistsError:
            pass
    if "--diskless" in arg_list:
        index = arg_list.index("--diskless")
        ret["diskless"] = True
        if "page-server" not in ret:
            ret["page-server"] = ("127.0.0.1", 1337)
    return ret

def transfer_checkpoint(**kwargs):
    # Only transfer if remote! Otherwise use --image-path flag.
    checkpoint_path = kwargs["container-path"] + "checkpoint"
    if (("restore-dir" in kwargs) and 
            (os.path.exists(checkpoint_path))):
        try:
            print("LOG: Transferring files to restore-dir...")
            shutil.copytree(checkpoint_path, kwargs["container-path"])
            print("LOG: Finished transferring files to restore-dir.")
        except (shutil.Error, OSError) as e:
            eprint("Couldn't transfer files. {}".format(e))
            sys.exit(1)

def prepare_diskless_migration(*kwargs):
    try:
        os.mkdir("/dev/shm/criu-src-dir")
        os.mkdir("/dev/shm/criu-dst-dir")
        cmd = "criu page-server --images-dir {} --port {}".format(
                "/dev/shm/criu-dst-dir", kwargs["page-server"][1])
        p = subprocess.Popen(cmd, shell = True)
        return p
    except FileExistsError:
        eprint("Environment not clean! Files in /dev/shm.")
        sys.exit(1)

def migration(**kwargs):
    # Checkpoint
    cmd_cp = "sudo runc checkpoint "
    cmd_rs = "sudo runc restore "
    if (kwargs["restore-type"] == "local"):
        cmd_cp += " --image-path {}".format(kwargs["image-path"])
        cmd_rs += " --image-path {}".format(kwargs["image-path"])
    else:
        # TODO remote migration
        #transfer_checkpoint(**kwargs)
        pass
    if (kwargs["diskless"])
        ps_p = prepare_diskless_migration(**kwargs)
        cmd_cp += " --page-server {}:{}"
    cmd_cp += " {}".format(kwargs["container-name"])
    print("Starting checkpoint with the following arguments:")
    print(kwargs)
    start = time.time()
    p = subprocess.Popen(cmd_cp, shell=True)
    p.wait()
    print("Checkpoint finished!")
    cp_time = time.time()
    cmd_rs += "-d {}-restored &> /dev/null < /dev/null".format(
            kwargs["container-name"])
    p = subprocess.Popen(cmd_rs, shell=True, cwd=kwargs["container-path"])
    p.wait()
    rt_time = time.time()
    print("Restore finished!")
    print("Time elapsed: \n\t- Checkpoint: {}\n\t- Restore: {}".format(
          cp_time, rt_time))

if __name__=="__main__":
    check_root()
    if len(sys.argv) < 2:
        usage()
    migration(**parse_kwargs(sys.argv))
