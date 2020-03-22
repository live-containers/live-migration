#include "migration.h"

/* To-Dos Before Releasing (or at least things to consider)
 *
 * 0. Update usage, READMEs and parse_args method
 * 1. Standarize Log Methods
 * 2. Use ints for flags.
 * 3. Put some sort of linter in place.
 * 4. Can we add unit tests?
 */

struct migration_args {
    ssh_session session;
    char *name;
    int iterative;
    char *src_image_path;
    char *dst_image_path;
    char *dst_host;
    char *dst_user;
    char *page_server_host;
    char *page_server_port;
};

int check_container_running(char *container_name)
{
    FILE *fp;
    char *cmd = "sudo runc list --format json | jq -r '.[] | select(.id == \"%s\")' | wc -l";
    char cmd2[MAX_CMD_SIZE];
    memset(cmd2, '\0', MAX_CMD_SIZE);
    sprintf(cmd2, cmd, container_name);

    fp = popen(cmd2, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "check_container_running: error running command '%s'\n", cmd2);
        return 1;
    }
    char result[16];
    memset(result, '\0', 16);
    fgets(result, 16, fp);
    if (atoi(result) == 0)
    {
        fprintf(stderr, "check_container_running: %s is not a running container.\n", container_name);
        pclose(fp);
        return 1;
    }
    pclose(fp);
    return 0;
}

int usage(char *file_name)
{
    printf("Usage: %s\n", file_name);
    printf("\t--name <container id>\tname of the runC container\n");
    printf("\t--image-path [dir]\tpath where to store the checkpoint files");
    printf("for non-diskless migrations\n"); //default /tmp/img-path
    printf("\t--diskless \t\tperform a diskless migration\n" );
    //txt += "\t--hostname <host:port>
    //#txt += "\t--container-path [dir|pwd]\t\tpath where the container image is\n"
    exit(1);
}

/* Parse Command Line Aruments if Running From the Command Line */
int parse_args(int argc, char *argv[], struct migration_args *args)
{
    if (argc < 2)
        usage(argv[0]);
    // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html#Getopt-Long-Options
    int c;
    while (1) 
    {
        static struct option long_options[] = 
        {
            {"name", required_argument, 0, 0},
            {"dst-host", required_argument, 0, 1},
            {"help", no_argument, 0, 99},
            {0, 0, 0, 0}
        };
        int option_index;
        c = getopt_long(argc, argv, "dni:", long_options, &option_index);
        if (c == -1)
            break; // No more args to parse.
        switch (c)
        {
            case 0:
                // FIXME check if container exists
                args->name = optarg;
                break;
            
            case 1:
                args->dst_host = optarg;
                break;

            case 99:
                usage(argv[0]);

            default:
                usage(argv[0]);
        }
    }

    if (args->name == NULL)
    {
        fprintf(stderr, "parse_args: you must specify a container to migrate.\n");
        usage(argv[0]);
    }
    else if (check_container_running(args->name) != 0)
    {
        fprintf(stderr, "parse_args: failed to verify running container.\n");
        usage(argv[0]);
    }
    if (args->dst_host == NULL)
        args->dst_host = "127.0.0.1";

    args->src_image_path = "/dev/shm/criu-src-dir/";
    args->dst_image_path = "/dev/shm/criu-dst-dir/";
    args->session = ssh_start("192.168.56.103", "carlos");
    return 0;
}

int launch_container(int experiment, char *experiment_tag)
{
    char cmd[MAX_CMD_SIZE];
    memset(cmd, '\0', MAX_CMD_SIZE);
    switch (experiment)
    {
        case EXPERIMENT_REDIS:
            sprintf(cmd, "cd %s && sudo runc run -d eureka &> /dev/null < /dev/null &",
                    RUNC_REDIS_PATH);            
            if (system(cmd) != 0)
            {
                fprintf(stderr, "launch_container: command '%s' failed.\n", cmd);
                return 1;
            }
            sleep(3);
            memset(cmd, '\0', MAX_CMD_SIZE);
            //sprintf(cmd, "cat \"%s/data/redis_%s.dat\" | redis-cli -h $(< %s/.ip) --pipe",
            //       RUNC_REDIS_PATH, experiment_tag, RUNC_REDIS_PATH);
            sprintf(cmd, "cd %s && ./run_redis.sh %s &> /dev/null",
                    RUNC_REDIS_PATH, experiment_tag);
            printf("DEBUG: redis cmd 1 -> '%s'\n", cmd);
            if (system(cmd) != 0)
            {
                fprintf(stderr, "launch_container: command '%s' failed.\n", cmd);
                return 1;
            }
            /*
            sleep(3);
            memset(cmd, '\0', MAX_CMD_SIZE);
            sprintf(cmd, "redis-cli -h $(< %s/.ip) config set \
                          stop-writes-on-bgsave-error no", RUNC_REDIS_PATH);
            printf("DEBUG: redis cmd 2 -> '%s'\n", cmd);
            if (system(cmd) != 0)
            {
                fprintf(stderr, "launch_container: redis config '%s' \
                                 failed.\n", cmd);
                return 1;
            }
            */
            return 0;

        default:
            fprintf(stderr, "launch_container: experiment %i not defined.\n",
                    experiment);
            return 1;
    }
    return 0;
}

/* Clean the working environment once we are done. */
int clean_env(struct migration_args *args)
{
    // TODO remove page-dir still alive in the remote end
    char rm_cmd[MAX_CMD_SIZE];
    memset(rm_cmd, '\0', MAX_CMD_SIZE);
    if (args->iterative)
        sprintf(rm_cmd, "rm -r criu-dst-*");
    else
        sprintf(rm_cmd, "rm -r %s", args->dst_image_path);
    if (ssh_remote_command(args->session, rm_cmd, 0) != SSH_OK)
    {
        fprintf(stderr, "clean_env: removing remote directory during cleanup failed.\n");
        return 1;
    }
    return 0;
}

/* Quick Set Up For Testing Purposes */
int init_migration(struct migration_args *args)
{
    args->name = "eureka";
    args->iterative = 1;
    args->dst_host = VM2_IP;
    args->dst_user = "carlos";
    args->page_server_host = "127.0.0.1";
    args->page_server_port = PAGE_SERVER_PORT;
    args->src_image_path = (char *) malloc(MAX_CMD_SIZE * sizeof(char));
    memset(args->src_image_path, '\0', MAX_CMD_SIZE);
    args->dst_image_path = (char *) malloc(MAX_CMD_SIZE * sizeof(char));
    memset(args->dst_image_path, '\0', MAX_CMD_SIZE);
    strcpy(args->src_image_path, "/dev/shm/criu-src-dir");
    strcpy(args->dst_image_path, "/dev/shm/criu-dst-dir");
    args->session = ssh_start(args->dst_host, args->dst_user);
    return 0;
}

/* Sets the environment up for a simple migration
 *
 * Creates local and remote dir from args, and runs the page server.
 * Recall that the page-server is a one-shot command, and need to be re
 * run everytime if we are doing a diskless migration.
 */
int prepare_migration(struct migration_args *args)
{
    int rc;
    /* Make local dir for checkpoint files */
    rc = mkdir(args->src_image_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (rc < 0)
    {
        fprintf(stderr, "prepare_migration: error creating local image path in SHM.\n");
        return 1;
    }
    /* Make remote dir for page server files */
    char rm_cmd[MAX_CMD_SIZE];
    memset(rm_cmd, '\0', MAX_CMD_SIZE);
    sprintf(rm_cmd, "mkdir %s", args->dst_image_path);
    if (ssh_remote_command(args->session, rm_cmd, 0) != SSH_OK)
    {
        fprintf(stderr, "prepare_migration: error creating dst dir.\n");
        return 1;
    }
    memset(rm_cmd, '\0', MAX_CMD_SIZE);
    sprintf(rm_cmd, "echo %s | sudo -S criu page-server -d --images-dir %s --port %s",
            REMOTE_PWRD, args->dst_image_path, args->page_server_port);
    if (ssh_remote_command(args->session, rm_cmd, 0) != SSH_OK)
    {
        fprintf(stderr, "prepare_migration: error initializing remote page server.\n");
        return 1;
    }
    return 0;
}

int iterative_migration_inc_dirs(struct migration_args *args, int level)
{
    int new_start;
    int num_size;
    switch (level)
    {
        case 0:
            /* Rename 
             *
             * If we get really picky w/ size use:
             * (int)((ceil(log10(num))+1)*sizeof(char))
             */
            strcat(args->src_image_path, "-0");
            strcat(args->dst_image_path, "-0");
            return 0;

        default:
            /* Here we use that memory is already allocated and nulled out. */
            num_size = (int)((floor(log10(level)) + 1) * sizeof(char));
            new_start = strlen(args->src_image_path) - num_size;
            /* Hardcoded limitat on the number of intermediate directories. */
            char num[5];
            memset(num, '\0', 5);
            sprintf(num, "%d", level);
            memset(args->src_image_path + new_start, '\0', num_size);
            strcat(args->src_image_path, num);
            memset(args->dst_image_path + new_start, '\0', num_size);
            strcat(args->dst_image_path, num);
            return 0;
    }
}

int iterative_migration(struct migration_args *args)
{
    /* Initialize the Recurrent Command we will Issue */
    int num_test_dumps = 10;
    char old_src_path[MAX_CMD_SIZE];
    char old_dst_path[MAX_CMD_SIZE];
    char cmd_db[MAX_CMD_SIZE];
    char cmd_dump[MAX_CMD_SIZE];
    char cmd_symlink[MAX_CMD_SIZE];
    /* FIXME fix this when a more realistic experiment is set */
    FILE *fp;
    char redis_ip[32];
    char filename[MAX_CMD_SIZE];
    memset(filename, '\0', MAX_CMD_SIZE);
    memset(redis_ip, '\0', 32);
    sprintf(filename, "%s/.ip", RUNC_REDIS_PATH);
    fp = fopen(filename, "r");
    if (fgets(redis_ip, 32, fp) == NULL)
    {
        fprintf(stderr, "iterative_migration: error getting Redis IP.\n");
        return 1;
    }
    fclose(fp);
    char *fmt_cmd_db = "cd %s && redis-benchmark -h %s -n 1000 && redis-cli \
                        -h %s SET iter iter%i";
    char *fmt_cmd_dump = "sudo runc checkpoint --pre-dump --image-path %s \
                          --parent-path %s --page-server %s:%s %s";
    char *fmt_cmd_symlink = "ln -s %s %s/parent";

    /* Start Iterative Page Dump 
     *
     * TODO Determine what threshold to use to trigger another iteration.
     * For the moment and for demonstrating purposes we just wait 3 seconds.
     */
    for (int i = 0; i <= num_test_dumps; i++)
    {
        printf("LOG: Iterative Migration --> Step %i.\n", i);
        /* Pre-Dump */
        if (prepare_migration(args) != 0)
        {
            fprintf(stderr, "iterative_migration: prepare migration failed at \
                             iteration %i.\n", i + 1);
            return 1;
        }
        memset(cmd_dump, '\0', MAX_CMD_SIZE);
        if (i == 0)
            sprintf(cmd_dump, "sudo runc checkpoint --pre-dump --image-path %s \
                    --page-server %s:%s %s", args->src_image_path, 
                    args->dst_host, args->page_server_port, args->name);
        else
            sprintf(cmd_dump, fmt_cmd_dump, args->src_image_path, old_src_path,
                    args->dst_host, args->page_server_port, args->name);
        if (system(cmd_dump) != 0)
        {
            fprintf(stderr, "iterative_migration: pre-dump #%i failed.\n", i);
            return 1;
        }

        /* Transfer the Remaining Files */
        if (sftp_copy_dir(args->session, args->dst_image_path, 
                          args->src_image_path, 1) != SSH_OK)
        {
            fprintf(stderr, "migration: error transferring from '%s' to '%s'\n",
                    args->src_image_path, args->dst_image_path);
            if (clean_env(args) != 0)
            {
                fprintf(stderr, "migration: clean_env method failed.\n");
                return 1;
            }
            return 1;
        }
        /* Generate the special parent symlink. */
        if (i > 0)
        {
            memset(cmd_symlink, '\0', MAX_CMD_SIZE);
            sprintf(cmd_symlink, fmt_cmd_symlink, old_dst_path,
                    args->dst_image_path);
            if (ssh_remote_command(args->session, cmd_symlink, 0) != SSH_OK)
            {
                fprintf(stderr, "iterative_migration: error creating symlink \
                                 w/ command: '%s'\n", cmd_symlink);
                return 1;
            }
        }

        /* Swap Dirs */
        memset(old_src_path, '\0', MAX_CMD_SIZE);
        strcpy(old_src_path, args->src_image_path);
        memset(old_dst_path, '\0', MAX_CMD_SIZE);
        strcpy(old_dst_path, args->dst_image_path);
        iterative_migration_inc_dirs(args, i);

        /* Run DB Command and Wait */
        printf("DEBUG: Running Redis Benchmark.\n");
        memset(cmd_db, '\0', MAX_CMD_SIZE);
        sprintf(cmd_db, fmt_cmd_db, RUNC_REDIS_PATH, redis_ip, redis_ip, i);
        if (system(cmd_db) != 0)
        {
            fprintf(stderr, "iterative_migration: db command %s failed.\n",
                    cmd_db);
            return 1;
        }

        /* Holdback time. FIXME how to choose this? */
        sleep(3);
    }
    return 0;
}

int migration(struct migration_args *args)
{
    /* Prepare Environment for Migration */
    if (args->iterative)
    {
        if (iterative_migration(args) != 0)
        {
            fprintf(stderr, "migration: iterative_migration failed.\n");
            if (clean_env(args) != 0)
            {
                fprintf(stderr, "migration: clean_env method failed.\n");
                return 1;
            }
            return 1;
        }
        printf("DEBUG: Finished testing iterative_migration.\n");
        return 0;
    }
    else
    {
        if (prepare_migration(args) != 0)
        {
            fprintf(stderr, "migration: prepare_migration failed.\n");
            if (clean_env(args) != 0)
            {
                fprintf(stderr, "migration: clean_env method failed.\n");
                return 1;
            }
            return 1;
        }
    }

    /* Craft Checkpoint and Restore Commands */
    char cmd_cp[MAX_CMD_SIZE];
    char cmd_rs[MAX_CMD_SIZE];
    memset(cmd_cp, '\0', MAX_CMD_SIZE);
    memset(cmd_rs, '\0', MAX_CMD_SIZE);
    char *fmt_cp = "sudo runc checkpoint --image-path %s --page-server \
                    %s:%s %s";
    char *fmt_rs = "cd %s && echo %s | sudo -S runc restore --image-path %s \
                    %s-restored &> /dev/null < /dev/null &";
    sprintf(cmd_cp, fmt_cp, args->src_image_path, args->dst_host,
            args->page_server_port, args->name);
    sprintf(cmd_rs, fmt_rs, RUNC_REDIS_PATH, REMOTE_PWRD, args->dst_image_path, args->name);

    /* Checkpoint the Running Container */
    if (system(cmd_cp) != 0)
    {
        fprintf(stderr, "migration: error checkpointing w/ command: '%s'\n",
                cmd_cp);
        if (clean_env(args) != 0)
        {
            fprintf(stderr, "migration: clean_env method failed.\n");
            return 1;
        }
        return 1;
    }

    /* Copy the Remaining Files (should be few as we are running diskless */
    if (sftp_copy_dir(args->session, args->dst_image_path, 
                      args->src_image_path, 1) != SSH_OK)
    {
        fprintf(stderr, "migration: error transferring from '%s' to '%s'\n",
                args->src_image_path, args->dst_image_path);
        if (clean_env(args) != 0)
        {
            fprintf(stderr, "migration: clean_env method failed.\n");
            return 1;
        }
        return 1;
    }

    /* Restore the Running Container */
    if (ssh_remote_command(args->session, cmd_rs, 0) != SSH_OK)
    {
        fprintf(stderr, "migration: error restoring w/ command: '%s'\n",
                cmd_rs);
        /* 
        if (clean_env(args) != 0)
        {
            fprintf(stderr, "migration: clean_env method failed.\n");
            return 1;
        }
        */
        return 1;
    }

    /* Clean Environment Before Exitting */
    sleep(1);
    if (clean_env(args) != 0)
    {
        fprintf(stderr, "migration: clean_env method failed.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    /* Check if running as root */
    if (getuid() != 0)
    {
        fprintf(stderr, "main: you need root privileges to run this program.\n");
        return 1;
    }

    /* DEBUG: Start Container */
    if (launch_container(EXPERIMENT_REDIS, "10") != 0)
    {
        fprintf(stderr, "main: launch_container failed.\n");
        return 1;
    }

    /* Argument Initialization */
    struct migration_args *args;
    args = (struct migration_args *) malloc(sizeof(struct migration_args));
    if (args == NULL)
    {
        printf("Error allocating command line arguments!\n");
        return 1;
    }
    // FIXME include all arguments when finished
    //parse_args(argc, argv, args);
    init_migration(args);

    /* Run Migration */
    migration(args);
    return 0;
}
