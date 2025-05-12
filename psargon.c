#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>

#define MAX_THREADS 16

char **create_ragged_array(const char *filename, int *line_count);
int count_lines(const char *file_name);

int main(int argc, char *argv)
{
    char *hash_file = NULL;
    char *password_file = NULL;
    char *output_file = NULL;
    char *log_file = NULL;
    int thread_count = 1;
    int verbose = 0;


    int opt
    while(opt = getopt(argc, argv, "h:p:o:l:t:vH"), opt != -1)
    {
        switch(opt)
        {
            case 'h':
                hash_file = optarg;
                break;
            case 'p':
                password_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'l':
                log_file = optarg;
                break;
            case 't':
                thread_count = atoi(optarg);
                if(thread_count < 1 || thread_count > MAX_THREADS)
                {
                    fprintf(stderr, "Thread count must be between 1 and %d\n", MAX_THREADS);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                verbose = 1;
                break;
            case 'H':  
                printf("Usage: %s -h hashfile -p passwordfile [options]\n", progname);
                printf("Options:\n");
                printf("  -h filename   Input file of Argon2 hashes (required)\n");
                printf("  -p filename   Input file of plaintext passwords (required)\n");
                printf("  -o filename   Output file (default: stdout)\n");
                printf("  -l filename   Log file (default: stderr)\n");
                printf("  -t number     Number of threads to use (default: 1, max: 16)\n");
                printf("  -v            Enable verbose logging\n");
                printf("  -H            Show this help message and exit\n");
            default:
                fprintf(stderr, "Usage: %s -h <hash_file> -p <password_file> -o <output_file> -l <log_file> -t <thread_count> -v\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if(!hash_file || !password_file) 
    {
        fprintf(stderr, "Error: -h and -p options are both required.\n");
        exit(EXIT_FAILURE);
    }































    return(EXIT_SUCCESS);
}