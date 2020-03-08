#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>

#define TRUST_UNKNOWN_HOSTS 1

int verify_host(ssh_session session)
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
            fprintf(stderr, "Could not find the key in the hosts file.\n");
            if (TRUST_UNKNOWN_HOSTS)
            {
                fprintf(stderr, "WARNING: Adding key to file.\n");
                fprintf(stderr, "To change this behaviour, compile with ");
                fprintf(stderr, "TRUST_UNKNOWN_HOSTS flag disabled.\n");
                /* Fall in the SSH_KNOWN_HOSTS_UNKNOWN case */
            }
            else
                ssh_clean_pubkey_hash(&hash);
                return -1;

        case SSH_KNOWN_HOSTS_UNKNOWN:
            ssh_clean_pubkey_hash(&hash);
            rc = ssh_session_update_known_hosts(session);
            if (rc < 0)
            {
                fprintf(stderr, "Error updating known hosts file!\n");
                return -1;
            }
            break;

        default:
            fprintf(stderr, "Can't handle case: %i in verify_host!\n",
                    state);
            return -1;
    }

    ssh_clean_pubkey_hash(&hash);
    return 0;
}

int authenticate_pubkey(ssh_session session)
{
    int rc;
    ssh_key privkey;
    char *key_path = "/home/csegarra/.ssh/id_rsa";

    /* Load Private Key */
    rc = ssh_pki_import_privkey_file(key_path, NULL, NULL, NULL, &privkey);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error loading private key!\n");
        ssh_key_free(privkey);
        return rc;
    }

    /* Authenticate Using Private Key */
    rc = ssh_userauth_publickey(session, NULL, privkey);
    if (rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "Authentication failed with error: %s\n",
                ssh_get_error(session));
        ssh_key_free(privkey);
        return SSH_AUTH_ERROR;
    }

    /* Free Resources and Return */
    ssh_key_free(privkey);
    return rc;
}

int run_remote_command(ssh_session session, char *command)
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;

    if (ssh_is_connected(session) == 1)
        fprintf(stdout, "We are connected!\n");

    /* Open a new SSH Channel */
    channel = ssh_channel_new(session);
    if (channel == NULL)
    {
        fprintf(stderr, "Error allocating new SSH channel.\n");
        return SSH_ERROR;
    }
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error opening new SSH channel.\n");
        ssh_channel_free(channel);
        return rc;
    }

    /* Execute Remote Command */
    rc = ssh_channel_request_exec(channel, command);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error executing remote command.\n");
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

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

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}

int sftp_copy(ssh_session session)
{
    sftp_session sftp;
    int rc;

    sftp = sftp_new(session);
    /* Allocate SFTP Session */
    if (sftp == NULL)
    {
        fprintf(stderr, "Error allocating SFTP session: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }

    /* Initialize SFTP Client */
    rc = sftp_init(sftp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing SFTP session: %d\n",
                sftp_get_error(sftp));
        sftp_free(sftp);
        return rc;
    }

    /* Create Test File */
    int access_type = O_WRONLY | O_CREAT | O_TRUNC;
    sftp_file file;
    const char *hello = "Hello, World!\n";
    int length = strlen(hello);
    int nwritten;

    file = sftp_open(sftp, "hello_world.txt", access_type, S_IRWXU);
    if (file == NULL)
    {
        fprintf(stderr, "Can't open file for writing: %s\n",
                ssh_get_error(session));
        sftp_free(sftp);
        return SSH_ERROR;
    }

    nwritten = sftp_write(file, hello, length);
    if (nwritten != length)
    {
        fprintf(stderr, "Can't write data to remote file: %s\n",
                ssh_get_error(session));
        sftp_close(file);
        sftp_free(sftp);
        return SSH_ERROR;
    }

    rc = sftp_close(file);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't close remote file: %s\n",
                ssh_get_error(session));
        sftp_free(sftp);
        return rc;
    }

    sftp_free(sftp);
    return SSH_OK;
}

int main()
{
    ssh_session session = ssh_new();
    int verbosity = SSH_LOG_PROTOCOL;
    int port = 22;
    char *host = "192.168.56.103";
    char *user = "carlos";
    int rc;
    char *known_hosts = "~/.ssh/known_hosts";

    if (session == NULL)
        exit(-1);
    
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
        fprintf(stderr, "Error connecting to %s: %s\n",
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

    /* Execute Remote Command */
    /*
    char *command = "cat setup.sh";
    if (run_remote_command(session, command) != SSH_OK)
    {
        fprintf(stderr, "Error executing remote command!\n");
        exit(-1);
    }
    */

    if (sftp_copy(session) != SSH_OK)
    {
        fprintf(stderr, "Error copying files over SFTP!\n");
        exit(-1);
    }

    ssh_free(session);
    return 0;
}
