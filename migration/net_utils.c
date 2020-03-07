#include <stdlib.h>
#include <stdio.h>
#include <libssh/libssh.h>

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

int show_remote_processes(ssh_session session)
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;

    /* Open a new SSH Channel */
    channel = ssh_channel_new(session);
    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }

    /* Execute Remote Command */
    rc = ssh_channel_request_exec(channel, "ps aux");
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    /* Read Output in chunks */
    nbytes = ssh_channel_read(channel, buffer, sizeof buffer, 0);
    while(nbytes > 0)
    {
        if (fprintf(stdout, buffer, nbytes) != (unsigned int) nbytes)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
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

int main()
{
    ssh_session session = ssh_new();
    int verbosity = SSH_LOG_PROTOCOL;
    int port = 22;
    char *host = "192.168.56.103";
    int rc;

    if (session == NULL)
        exit(-1);
    
    /* Set SSH Connection Options */
    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);

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
    /* TO-DO: Implement Client-Side Authentication
     * see here: http://api.libssh.org/master/libssh_tutor_guided_tour.html */

    /* Execute Remote Command */
    if (show_remote_processes(session) != SSH_OK)
    {
        fprintf(stderr, "Error executing remote command!\n");
        exit(-1);
    }

    ssh_free(session);
    return 0;
}
