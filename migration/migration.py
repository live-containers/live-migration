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
        tmp_out = subprocess.getoutput("sudo runc list --format json")
        cont_list = ast.literal_eval(tmp_out)
        ret["container-path"] = [i for i in cont_list
                            if i['id'] == ret["container-name"]][0]["bundle"]
    except IndexError:
        eprint("{} is not a running container!".format(
              ret["container-name"]))
        sys.exit(1)
    except Exception as e:
        print(e)
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
        # If diskless, override the image path. TODO check for possible mounts
        # 1. /dev/shm/
        ret["image-path"] = ("/dev/shm/criu-src-dir/", "/dev/shm/criu-dst-dir/")
        if "page-server" not in ret:
            ret["page-server"] = ("127.0.0.1", 1337)
    else:
        ret["diskless"] = False
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

def prepare_diskless_migration(**kwargs):
    try:
        os.mkdir(kwargs["image-path"][0])
        os.mkdir(kwargs["image-path"][1])
        cmd = "criu page-server --images-dir {} --port {}".format(
                kwargs["image-path"][1], kwargs["page-server"][1])
        p = subprocess.Popen(cmd, shell = True)
        return p
    except FileExistsError as e:
        print(e)
        eprint("Environment not clean! Files in /dev/shm.")
        sys.exit(1)

def clean_env(**kwargs):
    if (len(kwargs["image-path"]) == 2):
        try:
            shutil.rmtree(kwargs["image-path"][0])
        except Exception as e:
            print(e)
        try:
            shutil.rmtree(kwargs["image-path"][1])
        except Exception as e:
            print(e)
    else:
        if kwargs["image-path"].split('/')[1] == "tmp":
            try:
                shutil.rmtree(kwargs["image-path"])
            except Exception as e:
                print(e)

def migration(**kwargs):
    # Checkpoint
    cmd_cp = "sudo runc checkpoint "
    cmd_rs = "sudo runc restore "
    if (len(kwargs["image-path"]) == 2):
        cmd_cp += " --image-path {}".format(kwargs["image-path"][0])
        cmd_rs += " --image-path {}".format(kwargs["image-path"][1])
    else:
        cmd_cp += " --image-path {}".format(kwargs["image-path"])
        cmd_rs += " --image-path {}".format(kwargs["image-path"])
    if (kwargs["diskless"]):
        ps_p = prepare_diskless_migration(**kwargs)
        cmd_cp += " --page-server {}:{}".format(*kwargs["page-server"])
    cmd_cp += " {}".format(kwargs["container-name"])
#    print("Starting checkpoint with the following arguments:")
#    print(kwargs)
#    print(cmd_cp)
#    print(cmd_rs)
    start_time = time.time()
    p = subprocess.Popen(cmd_cp, shell=True)
    p.wait()
    print("Checkpoint finished!")
    if (kwargs["diskless"]):
        try:
            for _file in os.listdir(kwargs["image-path"][0]):
                shutil.copy("{}{}".format(kwargs["image-path"][0], _file),
                            kwargs["image-path"][1])
        except Exception as e:
            print(e)
            eprint("Can't copy from src to dst tmpfs!")
            clean_env(**kwargs)
            sys.exit(1)
    cp_time = time.time()
    cmd_rs += " -d {}-restored &> /dev/null < /dev/null".format(
            kwargs["container-name"])
    p = subprocess.Popen(cmd_rs, shell=True, cwd=kwargs["container-path"])
    p.wait()
    rt_time = time.time()
    print("Restore finished!")
    print("Time elapsed: \n\t- Checkpoint: {}\n\t- Restore: {}".format(
          cp_time - start_time, rt_time - cp_time))
    print("\t- Total: {}".format(rt_time - start_time))
    print("Cleaning environment before shutdown!")
    time.sleep(2)
    clean_env(**kwargs)

if __name__=="__main__":
    check_root()
    if len(sys.argv) < 2:
        usage()
    migration(**parse_kwargs(sys.argv))
