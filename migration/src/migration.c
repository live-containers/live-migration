#include "migration.h"

struct migration_args {
    ssh_session session;
    char *name;
    char *src_image_path;
    char *dst_image_path;
    char *dst_host;
    char *page_server_host;
    char *page_server_port;
};

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
        printf("You must specify a container to migrate!\n");
        usage(argv[0]);
    }
    if (args->dst_host == NULL)
        args->dst_host = "127.0.0.1";

    args->src_image_path = "/dev/shm/criu-src-dir/";
    args->dst_image_path = "/dev/shm/criu-dst-dir/";
    return 0;
}

int prepare_migration(struct migration_args *args)
{
    int rc;
    /* Make local dir for checkpoint files */
    rc = mkdir(args->src_image_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (rc < 0)
    {
        printf("Error creating local image path in SHM!\n");
        return 1;
    }

    return 0;
}

int migration(struct migration_args *args)
{
    char *cmd_cp = NULL;
    char *cmd_rs = NULL;
    char *fmt_cp = "sudo runc checkpoint --image-path %s --page-server %s:%s %s";
    char *fmt_rs = "sudo runc checkpoint --image-path %s %s-restored &> /dev/null < /dev/null";
    sprintf(cmd_cp, fmt_cp, args->src_image_path, args->page_server_host,
            args->page_server_port, args->name);
    sprintf(cmd_rs, fmt_rs, args->dst_image_path, args->name);
    if (prepare_migration(args) != 0)
        return 1;

    return 0;
}

int main(int argc, char *argv[])
{
    if (getuid() != 0)
    {
        printf("You need to be root to run this program!\n");
        return 1;
    }
    struct migration_args *args;
    args = (struct migration_args *) malloc(sizeof(struct migration_args));
    if (args == NULL)
    {
        printf("Error allocating command line arguments!\n");
        return 1;
    }
    parse_args(argc, argv, args);
    args->session = ssh_start("192.168.56.103", "carlos");
    char *command = "cat setup.sh";
    if (ssh_remote_command(args->session, command) != SSH_OK)
    {
        fprintf(stderr, "Error executing remote command!\n");
        exit(-1);
    }
    //migration(args);
    return 0;
}
