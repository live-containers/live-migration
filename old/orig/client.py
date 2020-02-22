#!/usr/bin/python3
import socket
import sys
import select
import time
import os
import shutil
import subprocess
import distutils.util

runc_base = "/runc/containers/"

lazy = False
pre = False

if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " [container id] [destination] [pre-copy] [post-copy]"
    sys.exit(1)

container = sys.argv[1]
dest = sys.argv[2]
if len(sys.argv) > 3:
    pre = distutils.util.strtobool(sys.argv[3])
if len(sys.argv) > 4:
    lazy = distutils.util.strtobool(sys.argv[4])

base_path = runc_base + container
image_path = base_path + "/image"
parent_path = base_path + "/parent"

rsync_opts = "-aqz"

def error():
    print "Something did not work. Exiting!"
    sys.exit(1)

def prepare():
    try:
        shutil.rmtree(image_path)
    except:
        pass
    try:
        shutil.rmtree(parent_path)
    except:
        pass

def pre_dump():
    old_cwd = os.getcwd()
    os.chdir(base_path)
    cmd = 'runc checkpoint  --pre-dump --image-path parent'
    cmd += ' ' + container
    start = time.time()
    ret = os.system(cmd)
    end = time.time()
    print "%s finished after %d second(s) with %d" % (cmd, end - start, ret)
    os.chdir(old_cwd)
    if ret != 0:
        error()

def real_dump(precopy, postcopy):
    old_cwd = os.getcwd()
    os.chdir(base_path)
    cmd = 'runc checkpoint --image-path image --leave-running'
    cmd = 'runc checkpoint --image-path image '
    if precopy:
        cmd += ' --parent-path ../parent'
    if postcopy:
        cmd += ' --lazy-pages'
        cmd += ' --page-server localhost:27'
	try:
	    os.unlink('/tmp/postcopy-pipe')
        except:
            pass
        os.mkfifo('/tmp/postcopy-pipe')
	cmd += ' --status-fd /tmp/postcopy-pipe'
    cmd += ' ' + container
    start = time.time()
    print cmd
    p = subprocess.Popen(cmd, shell=True)
    if postcopy:
        p_pipe = os.open('/tmp/postcopy-pipe', os.O_RDONLY)
	ret = os.read(p_pipe, 1)
        if ret == '\0':
            print 'Ready for lazy page transfer'
	ret = 0
    else:
        ret = p.wait()

    end = time.time()
    print "%s finished after %.2f second(s) with %d" % (cmd, end - start, ret)
    os.chdir(old_cwd)
    if ret != 0:
        error()

def xfer_pre_dump():
    sys.stdout.write('PRE-DUMP size: ')
    sys.stdout.flush()
    cmd = 'du -hs %s' % parent_path
    ret = os.system(cmd)
    cmd = 'rsync %s %s %s:%s/' % (rsync_opts, parent_path, dest, base_path)
    print "Transferring PRE-DUMP to %s" % dest
    start = time.time()
    ret = os.system(cmd)
    end = time.time()
    print "PRE-DUMP transfer time %s seconds" % (end - start)
    if ret != 0:
        error()

def xfer_final():
    sys.stdout.write('DUMP size: ')
    sys.stdout.flush()
    cmd = 'du -hs %s' % image_path
    ret = os.system(cmd)
    cmd = 'rsync %s %s %s:%s/' % (rsync_opts, image_path, dest, base_path)
    print "Transferring DUMP to %s" % dest
    start = time.time()
    ret = os.system(cmd)
    end = time.time()
    print "DUMP transfer time %s seconds" % (end - start)
    if ret != 0:
        error()



def touch(fname):
    open(fname, 'a').close()


def give_ip():
    print "Giving floating IP to " + dest
    touch('/tmp/give_up_master_99')
    cs = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cs.connect((dest, 8888))
    cs.send('{ "take_ip" : "/tmp/give_up_master_99" }')
    os.system('systemctl stop keepalived')
    cs.close()

prepare()
if pre:
    pre_dump()
    xfer_pre_dump()
real_dump(pre, lazy)
give_ip()
xfer_final()

cs = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cs.connect((dest, 8888))

input = [cs,sys.stdin]


cs.send('{ "restore" : { "path" : "' + base_path + '", "name" : "' + container + '" , "image_path" : "' + image_path + '" , "lazy" : "' + str(lazy) + '" } }')

while True:
    inputready, outputready, exceptready = select.select(input,[],[], 5)

    if not inputready:
        break

    for s in inputready:
         answer = s.recv(1024)
         print answer

