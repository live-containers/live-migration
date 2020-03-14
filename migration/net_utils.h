#ifndef NET_UTILS_H
#define NET_UTILS_H
#include <fcntl.h>
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
int verify_host(ssh_session session);
int authenticate_pubkey(ssh_session session);
int run_remote_command(ssh_session session, char *command);
int sftp_copy(ssh_session session, char *dst_path, char *src_path);

#endif /* NET_UTILS_H */
