#include "migration.h"

struct migration_args {
    char *name;
    char *image_path;
    int diskless;
};

static int file_name_flag;

int usage(char *file_name)
{
    printf("Usage: %s\n", file_name);
    printf("\t--name <container id>\t\tname of the runC container\n");
    printf("\t--image-path [dir]\t\tpath where to store the checkpoint files");
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
            {"diskless", no_argument, 0, 'd'},
            {"name", required_argument, &file_name_flag, 1},
            {"image-path", optional_argument, 0, 'i'},
            {"help", optional_argument, 0, 'h'},
            {0, 0, 0, 0}
        };
        int option_index;
        c = getopt_long(argc, argv, "dni:", long_options, &option_index);
        if (c == -1)
            break; // No more args to parse.
        switch (c)
        {
            case 0:
                if (optarg)
                {
                    args->name = optarg;
                    break;
                }
                else
                    usage(argv[0]);
            
            case 'd':
                args->diskless = 1;
                break;

            case 'i':
                args->image_path = optarg;
                break;

            case 'h':
                usage(argv[0]);

            default:
                usage(argv[0]);
        }
    }

    if (!file_name_flag)
    {
        printf("You must set the --name argument!\n");
        usage(argv[0]);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct migration_args *args;
    args = (struct migration_args *) malloc(sizeof(struct migration_args));
    if (args == NULL)
    {
        printf("Error allocating command line arguments!\n");
        return 1;
    }
    parse_args(argc, argv, args);
    return 0;
}
