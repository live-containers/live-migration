#!/usr/bin/python3
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
    txt += "\t--restore-dir <dir>\t\tscp-like path where to restore "
    txt += "the container\n"
    #txt += "\t--container-path [dir|pwd]\t\tpath where the container image is\n"
    txt += "\t--diskless [bool|false]\t\tperform a diskless migration\n" 
    #txt += "[leave-running] [pre-copy|post-copy]\n"
    eprint(txt)
    sys.exit(1)

def parse_kwargs(arg_list):
    ret = {}
    if "--name" not in arg_list:
        eprint("Missing mandatory argument --name!")
        usage()
    try:
        index = arg_list.index("--name")
        ret["container-name"] = arg_list[index + 1]
        tmp_out = subprocess.getoutput("sudo runc list -q").split('\n')
        if ret["container-name"] not in tmp_out:
            raise CustomException
    except CustomException:
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
    if "--restore-dir" in arg_list:
        # TODO implement remote migration
        try:
            index = arg_list.index("--restore-dir")
            ret["restore-dir"] = arg_list[index + 1]
            ret["restore-type"] = "local"
            if (ret["restore-dir"][-1] != "/"):
                ret["restore-dir"] += "/"
            ret["restore-parent"] = ret["restore-dir"].rsplit('/',2)[0]
            if not os.path.exists(ret["restore-parent"]):
                eprint("{} is not a valid restore path!".format(
                      ret["restore-dir"]))
                raise CustomException
            if not os.path.exists(ret["restore-parent"] + "/config.json"):
                tmp_str = "{} has not got the sufficient ".format(
                        ret["container-path"])
                tmp_str += "files to restore the container!"
                eprint(tmp_str)
                raise CustomException
        except CustomException:
            sys.exit(1)
        except:
            eprint("In the restore-dir command")
            usage()
    else:
        eprint("Missing mandatory argument --restore-dir!")
        usage()
    if "--container-path" in arg_list:
        try:
            index = arg_list.index("--container-path")
            ret["container-path"] = arg_list[index + 1]
            if (ret["container-path"][-1] != "/"):
                ret["container-path"] += "/"
            if not os.path.exists(ret["container-path"]):
                eprint("{} is not a valid restore path!".format(
                      ret["restore-dir"]))
                raise CustomException
            if not os.path.exists(ret["container-path"] + "config.json"):
                tmp_str = "{} has not got the sufficient ".format(
                        ret["container-path"])
                tmp_str += "files to restore the container!"
                eprint(tmp_str)
                raise CustomException
        except CustomException:
            sys.exit(1)
        except:
            eprint("In the container path argument.")
            usage()
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

def migration(**kwargs):
    # Checkpoint
    cmd_cp = "sudo runc checkpoint {} ".format(kwargs["container-name"])
    cmd_rs = "sudo runc restore "
    if (kwargs["restore-type"] == "local"):
        cmd_cp += " --image-path {} ".format(kwargs["restore-dir"])
        cmd_rs += " --image-path {} ".format(kwargs["restore-dir"])
    else:
        # TODO remote migration
        #transfer_checkpoint(**kwargs)
        pass
    print("Starting checkpoint with the following arguments:")
    print(kwargs)
    start = time.time()
    p = subprocess.Popen(cmd_cp, shell=True)
    p.wait()
    print("Checkpoint finished!")
    cp_time = time.time()
    cmd_rs += "-d {}-restored &> /dev/null < /dev/null".format(
            kwargs["container-name"])
    p = subprocess.Popen(cmd_rs, shell=True, cwd=kwargs["restore-parent"])
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
