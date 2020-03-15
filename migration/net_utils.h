#ifndef NET_UTILS_H
#define NET_UTILS_H
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>

// General Configuration
#define TRUST_UNKNOWN_HOSTS 1

// Networking Parameters
#define HOST_IP             "192.168.56.1" 
#define VM1_IP              "192.168.56.101"
#define VM2_IP              "192.168.56.103"

// Other Constants
#define MAX_XFER_BUF_SIZE   10240

// Method Signatures
ssh_session ssh_start(char *host, char *user);

int sftp_copy_dir(ssh_session session, char *dst_path, char *src_path);
int sftp_copy_file(ssh_session session, char *dst_path, char *src_path);
int ssh_remote_command(ssh_session session, char *command);

#endif /* NET_UTILS_H */
