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
#define PAGE_SERVER_PORT    "1337"

// Other Constants
#define MAX_XFER_BUF_SIZE   10240
#define MAX_CMD_SIZE        512
#define REMOTE_PWRD         "csegarra1234" // L0L

// Method Signatures
ssh_session ssh_start(char *host, char *user);

int sftp_copy_dir(ssh_session session, char *dst_path, char *src_path, int rm_ori);
int sftp_copy_file(ssh_session session, char *dst_path, char *src_path);
int ssh_remote_command(ssh_session session, char *command, int read_output);

#endif /* NET_UTILS_H */
