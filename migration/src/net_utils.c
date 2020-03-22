#include "net_utils.h"

/*
 * Check whether the current session is established with a known host,
 * i.e. a host whose key is in the known_hosts file. If not, the current
 * behaviour is to add said key to the file, but it can change.
 */
static int verify_host(ssh_session session)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = NULL;
    size_t hlen;
    int rc;
    ssh_key srv_pubkey = NULL;

    /* Get Server's Public Key */
    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0)
    {
        return -1;
    }

    /* Get Hash from Public Key */
    rc = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1,
                                &hash, &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0)
        return -1;

    /* Check if host we are connecting to is known */
    state = ssh_session_is_known_server(session);
    switch (state)
    {
        case SSH_KNOWN_HOSTS_OK:
            /* Key in ~/.ssh/known_hosts file */
            break;

        case SSH_KNOWN_HOSTS_NOT_FOUND:
            fprintf(stderr, "verify_host: Could not find the key in the hosts file.\n");
            if (TRUST_UNKNOWN_HOSTS)
            {
                fprintf(stderr, "WARNING: Adding key to file.\n");
                fprintf(stderr, "To change this behaviour, compile with ");
                fprintf(stderr, "TRUST_UNKNOWN_HOSTS flag disabled.\n");
                /* Fall in the SSH_KNOWN_HOSTS_UNKNOWN case */
            }
            else
            {
                ssh_clean_pubkey_hash(&hash);
                return -1;
            }

        case SSH_KNOWN_HOSTS_UNKNOWN:
            ssh_clean_pubkey_hash(&hash);
            rc = ssh_session_update_known_hosts(session);
            if (rc < 0)
            {
                fprintf(stderr, "verify_host: Error updating known hosts file!\n");
                return -1;
            }
            break;

        default:
            fprintf(stderr, "verify_host: Can't handle case: %i in verify_host!\n",
                    state);
            return -1;
    }

    ssh_clean_pubkey_hash(&hash);
    return 0;
}

static int authenticate_pubkey(ssh_session session)
{
    int rc;
    ssh_key privkey;
    char *key_path = "/home/carlos/.ssh/id_rsa";

    /* Load Private Key */
    rc = ssh_pki_import_privkey_file(key_path, NULL, NULL, NULL, &privkey);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "authenticate_pubkey: Error loading private key!\n");
        ssh_key_free(privkey);
        return rc;
    }

    /* Authenticate Using Private Key */
    rc = ssh_userauth_publickey(session, NULL, privkey);
    if (rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "authenticate_pubkey: Authentication failed with error: %s\n",
                ssh_get_error(session));
        ssh_key_free(privkey);
        return SSH_AUTH_ERROR;
    }

    /* Free Resources and Return */
    ssh_key_free(privkey);
    return rc;
}

int ssh_remote_command(ssh_session session, char *command, int read_output)
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;

    /* Open a new SSH Channel */
    channel = ssh_channel_new(session);
    if (channel == NULL)
    {
        fprintf(stderr, "ssh_remote_command: Error allocating new SSH channel.\n");
        return SSH_ERROR;
    }
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "ssh_remote_command: Error opening new SSH channel.\n");
        ssh_channel_free(channel);
        return rc;
    }

    /* Execute Remote Command 
     *
     * We need to run the commands as sudo in the remote system as well
     * (criu needs to run as root) so I thought of two different ways
     * of tackling the problem:
     * 1. Passing the password as plain text.
     * 2. Manually setup each host to allow rootless sudo.
     * */
    /*
    char sudo_command[MAX_CMD_SIZE];
    memset(sudo_command, '\0', MAX_CMD_SIZE);
    sprintf(sudo_command, "echo %s | sudo -S %s", REMOTE_PWRD, command);
    */
    rc = ssh_channel_request_exec(channel, command);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "ssh_remote_command: Error executing remote command: %s\n",
                command);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    /* Check the Exit Status of the Remote Command */
    rc = ssh_channel_get_exit_status(channel);
    switch (rc) 
    {
        case 0:
            printf("DEBUG: command '%s' exitted succesfully!\n", command);
            break;

        case -1:
            printf("DEBUG: still no exit code received!\n");
            break;

        default:
            fprintf(stderr, "ssh_remote_command: remote command '%s' failed w/ exit status %i\n",
                    command, rc);
            return SSH_ERROR;
    }

    if (read_output)
    {
        /* Read Output in chunks */
        nbytes = ssh_channel_read(channel, buffer, sizeof buffer, 0);
        while(nbytes > 0)
        {
            fprintf(stdout, "%s", buffer);
            /* FIXME check for errors
            if (fprintf(stdout, "%s", buffer) != (unsigned int) nbytes)
            {
                fprintf(stderr, "Error printing results.\n");
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                return SSH_ERROR;
            }
            */
            nbytes = ssh_channel_read(channel, buffer, sizeof buffer, 0);
        }
        
        if (nbytes < 0)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}

/* Low-level helper function to transfer a specific file by chunks.
 * TODO how could we leverage compression in this case?
 */
static int sftp_xfer_file(sftp_session sftp, char *dst_path, char *src_path)
{
    FILE *src_file;
    struct stat src_stat;
    sftp_file dst_file;
    int rc;

    /* Open remote and local file.
     * First flag can be: O_WRONLY, O_RDONLY, or O_RDWR
     * Second flag can be: O_CREAT to create a new file
     * Third flag can be (if second set): O_EXCL (error if file exists)
     *                                    O_TRUNC (truncate if it exists)
    */
    int access_type = O_WRONLY | O_CREAT | O_TRUNC;
    src_file = fopen(src_path, "r");
    if (src_file == NULL)
    {
        fprintf(stderr, "sftp_xfer_file: Can't open the source file!\n");
        sftp_free(sftp);
        return SSH_ERROR;
    }
    /* Read src file permissions, the new file will have the same ones. */
    if (stat(src_path, &src_stat) < 0) {
        fprintf(stderr, "sftp_xfer_file: Can't read the source file's permission!\n");
        sftp_free(sftp);
        return SSH_ERROR;
    }
    /* If we are trying to copy a directory, do nothing. */
    if (!S_ISREG(src_stat.st_mode))
    {
        fprintf(stderr, "sftp_xfer_file: Trying to transfer a directory: %s\n",
                src_path);
        return SSH_ERROR;
    }
    dst_file = sftp_open(sftp, dst_path, access_type, 0666);
    //dst_file = sftp_open(sftp, dst_path, access_type, src_stat.st_mode);
    if (dst_file == NULL)
    {
        fprintf(stderr, "sftp_xfer_file: Can't open the destination file: %d\n",
                sftp_get_error(sftp));
        sftp_free(sftp);
        return SSH_ERROR;
    }

    /* Transfer data in chunks. */
    size_t nread, nwritten;
    char buffer[MAX_XFER_BUF_SIZE];
    while ((nread = fread(buffer, sizeof(char), MAX_XFER_BUF_SIZE, src_file)) > 0)
    {
        nwritten = sftp_write(dst_file, buffer, nread);
        if (nwritten != nread)
        {
            fprintf(stderr, "sftp_xfer_file: Can't write data to remote file: %d\n",
                    sftp_get_error(sftp));
            sftp_close(dst_file);
            sftp_free(sftp);
            return SSH_ERROR;
        }
    }

    fclose(src_file);
    rc = sftp_close(dst_file);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "sftp_xfer_file: Can't close remote file: %d\n",
                sftp_get_error(sftp));
        sftp_free(sftp);
        return rc;
    }
    return SSH_OK;
}

int sftp_copy_file(ssh_session session, char *dst_path, char *src_path)
{
    sftp_session sftp;
    int rc;

    sftp = sftp_new(session);
    /* Allocate SFTP Session */
    if (sftp == NULL)
    {
        fprintf(stderr, "sftp_copy_file: Error allocating SFTP session: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }

    /* Initialize SFTP Client */
    rc = sftp_init(sftp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "sftp_copy_file: Error initializing SFTP session: %d\n",
                sftp_get_error(sftp));
        sftp_free(sftp);
        return rc;
    }

    if (sftp_xfer_file(sftp, dst_path, src_path) != SSH_OK)
        return SSH_ERROR;

    sftp_free(sftp);
    return SSH_OK;
}

/* Copy the Contents of the Source Directory to the Destination One 
 *
 * ssh_session session: current authenticated ssh_session.
 * char *dst_path: path to the (existing) destination directory.
 * char *ori_path: path to the (existing) origin directory from where to copy.
 * int rm_ori: if set to 1, it will remove the contents of the origin directory.
 */
int sftp_copy_dir(ssh_session session, char *dst_path, char *src_path, int rm_ori)
{
    sftp_session sftp;
    int rc;

    sftp = sftp_new(session);
    /* Allocate SFTP Session */
    if (sftp == NULL)
    {
        fprintf(stderr, "sftp_copy_dir: Error allocating SFTP session: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }

    /* Initialize SFTP Client */
    rc = sftp_init(sftp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "sftp_copy_dir: Error initializing SFTP session: %d\n",
                sftp_get_error(sftp));
        sftp_free(sftp);
        return rc;
    }

    /* Iterate over source directory. */
    DIR *d;
    struct dirent *src_dir;
    //struct stat src_stat;
    d = opendir(src_path);
    if (d)
    {
        /* Create remote copy of directory. */
        /* TODO make this optional?
        if (sftp_mkdir(sftp, dst_path, 0755) != 0)
        {
            fprintf(stderr, "sftp_copy_dir: Error creating remore directory %d\n",
                    sftp_get_error(sftp));
            sftp_free(sftp);
            return SSH_ERROR;
        }
        */
        char resolved_path[PATH_MAX + 1];
        char src_rel_path[PATH_MAX + 1], dst_rel_path[PATH_MAX + 1];
        memset(src_rel_path, '\0', PATH_MAX + 1);
        memset(dst_rel_path, '\0', PATH_MAX + 1);
        memset(resolved_path, '\0', PATH_MAX + 1);
        while ((src_dir = readdir(d)) != NULL)
        {
            if (src_dir->d_type == DT_REG)
            {
                /* Generate full paths */
                strncpy(src_rel_path, src_path, strlen(src_path));
                strcat(src_rel_path, "/");
                strcat(src_rel_path, src_dir->d_name);
                strncpy(dst_rel_path, dst_path, strlen(dst_path));
                printf("DEBUG: dst_rel_path: %s\n", dst_rel_path);
                strcat(dst_rel_path, "/");
                printf("DEBUG: dst_rel_path: %s\n", dst_rel_path);
                strcat(dst_rel_path, src_dir->d_name);
                printf("DEBUG: dst_rel_path: %s\n", dst_rel_path);
                if (realpath(src_rel_path, resolved_path) == NULL)
                {
                    fprintf(stderr, "sftp_copy_dir: Error obtaining file's real path: %s\n",
                            src_dir->d_name);
                    sftp_free(sftp);
                    return SSH_ERROR;
                }
                if (sftp_xfer_file(sftp, dst_rel_path, resolved_path) != SSH_OK)
                {
                    fprintf(stderr, "sftp_copy_dir: error copying %s to %s\n. %i\n",
                            resolved_path, dst_rel_path, sftp_get_error(sftp));
                    sftp_free(sftp);
                    return SSH_ERROR;
                }
                if (remove(resolved_path) != 0)
                {
                    fprintf(stderr, "sftp_copy_dir: error removing local file %s (remove flag set)\n",
                            resolved_path);
                    sftp_free(sftp);
                    return SSH_ERROR;
                }
                memset(src_rel_path, '\0', PATH_MAX + 1);
                memset(dst_rel_path, '\0', PATH_MAX + 1);
                memset(resolved_path, '\0', PATH_MAX + 1);
            }
            else if (src_dir->d_type == DT_LNK)
            {
                /* On iterative migration, each intermediate checkpoint dir
                 * has a symbolic link to its "parent". Copying it
                 * programatically is more verbose than crafting it ourselves.
                 */
                strncpy(src_rel_path, src_path, strlen(src_path));
                strcat(src_rel_path, "/");
                strcat(src_rel_path, src_dir->d_name);
                if (remove(src_rel_path) != 0)
                {
                    fprintf(stderr, "sftp_copy_dir: error removing \
                                     symlink.\n");
                    return 1;
                }
                memset(src_rel_path, '\0', PATH_MAX + 1);
            }
        }
        closedir(d);
    }
    else
    {
        fprintf(stderr, "sftp_copy_dir: Error listing source directory!\n");
        sftp_free(sftp);
        return SSH_ERROR;
    }

    if (rm_ori && (rmdir(src_path) != 0))
    {
        fprintf(stderr, "sftp_copy_dir: failed removing origin directory \
                         '%s'\n", src_path);
        sftp_free(sftp);
        return SSH_ERROR;
    }
    sftp_free(sftp);
    return SSH_OK;
}

ssh_session ssh_start(char *host, char *user)
{
    ssh_session session = ssh_new();
    // SSH_LOG_NOLOG, SSH_LOG_WARNING, SSH_LOG_PROTOCOL, SSH_LOG_PACKET, SSH_LOG_FUNCTIONS
    int verbosity = SSH_LOG_NOLOG;
    int port = 22;
    int rc;
    char *known_hosts = "~/.ssh/known_hosts";

    if (session == NULL)
    {
        printf("Error allocating SSH Session!\n");
        exit(1);
    }

    /* Set SSH Connection Options */
    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_USER, user);
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_KNOWNHOSTS, known_hosts);
    ssh_options_set(session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS, known_hosts);

    /* Connect to Remote Server */
    rc = ssh_connect(session);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "ssh_start: Error connecting to %s: %s\n",
                host, ssh_get_error(session));
        ssh_free(session);
        exit(-1);
    }

    /* Authenticate Server Against Known Hosts */
    if (verify_host(session) < 0)
    {
        ssh_disconnect(session);
        ssh_free(session);
        exit(-1);
    }
    /* FIXME this requires the keys to be pre-shared. */
    if (authenticate_pubkey(session) != SSH_AUTH_SUCCESS)
    {
        exit(-1);
    }

    return session;
}

/*
int main()
{
    ssh_session session = ssh_start("192.168.56.103", "carlos");

    char *command = "cat setup.sh";
    if (ssh_remote_command(session, command) != SSH_OK)
    {
        fprintf(stderr, "Error executing remote command!\n");
        exit(-1);
    }

    //char *src_path = "/home/csegarra/Work/VIRT/criu-lm/migration/hello_world.txt";
    //char *dst_path = "hello_world.txt";
    char *src_path = "/home/csegarra/Work/VIRT/dissemination/figures/local-benchmark/local_benchmark.pdf";
    char *dst_path = "Desktop/hello_world.pdf";
    if (sftp_copy_file(session, dst_path, src_path) != SSH_OK)
    {
        fprintf(stderr, "Error copying files over SFTP!\n");
        exit(-1);
    }
    char *src_path = "test";
    char *dst_path = "lala";
    if (sftp_copy_dir(session, dst_path, src_path) != SSH_OK)
    {
        fprintf(stderr, "Error copying dir over SFTP!\n");
        exit(-1);
    }

    ssh_free(session);
    return 0;
}
*/
