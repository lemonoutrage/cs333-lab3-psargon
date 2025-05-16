#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

#define MAX_THREADS 16
#define DEFAULT_THREAD_COUNT 1
#define DEFAULT_VERBOSE 0

typedef struct {
    int thread_id;
    int hash_count;
    char **hashes;
    int password_count;
    char **passwords;
    int cracked_count;
    int failed_count;
    FILE *output_fp;
    FILE *log_fp;
    int verbose;
} threaddata;
//prototypes
static int count_lines(const char *string);
static char **create_line_array(char *str, int lines);
char **create_ragged_array(const char *filename, int *file_count);
void setup_logging(FILE **log_fp, const char *log_file, int verbose);
void setup_output(FILE **output_fp, const char *output_file);
void *thread_worker(void *arg);


static int count_lines(const char *string) {
    int count = 0;
    while(*string) {
        if (*string == '\n') {
            count++;
        }
        string++;
    }
    return count;
}

static char **create_line_array(char *str, int lines) {
    char **arr = malloc(lines * sizeof(char *));
    int index = 0;
    char *line = strtok(str, "\n");
    while(line && index < lines) {
        arr[index++] = line;
        line = strtok(NULL, "\n");
    }
    return arr;
}
char **create_ragged_array(const char *filename, int *file_count)
{
    char **arr = NULL;
    char *file_buffer = NULL;
    struct stat st;
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return NULL;
    }
    if (fstat(fd, &st) != 0) {
        perror("Failed to get file size");
        close(fd);
        return NULL;
    }

    file_buffer = calloc(st.st_size + 1, 1);
    if (!file_buffer) {
        perror("Failed to allocate memory for file buffer");
        close(fd);
        return NULL;
    }
    {
        ssize_t bytes_read = read(fd, file_buffer, st.st_size);
        if (bytes_read != st.st_size) {
            perror("Failed to read file");
            free(file_buffer);
            close(fd);
            return NULL;
        }
        close(fd);
    }
    *file_count = count_lines(file_buffer);
    if(*file_count == 0) {
        free(file_buffer);
        return NULL;
    }
    arr = create_line_array(file_buffer, *file_count);
    return arr;



}
void setup_logging(FILE **log_fp, const char *log_file, int verbose) {
    if (log_file) {
        *log_fp = fopen(log_file, "w");
        if (!*log_fp) {
            fprintf(stderr, "Failed to open log file: %s\n", log_file);
            *log_fp = stderr;
        }
    } else {
        *log_fp = stderr;
    }
    if (verbose) {
        fprintf(*log_fp, "Verbose logging enabled\n");
    }
}

void setup_output(FILE **output_fp, const char *output_file) {
    if (output_file) {
        *output_fp = fopen(output_file, "w");
        if (!*output_fp) {
            fprintf(stderr, "Failed to open output file: %s\n", output_file);
            *output_fp = stdout;
        }
    } else {
        *output_fp = stdout;
    }
}

void *thread_worker(void *arg) {
    threaddata *td = (threaddata *)arg;
    if (td->verbose) {
        fprintf(td->log_fp, "Thread %d starting work\n", td->thread_id);
    }
    // Thread processing logic will go here
    return NULL;
}

int main(int argc, char *argv[]) {
    char *hash_file = NULL;
    char *password_file = NULL;
    char *output_file = NULL;
    char *log_file = NULL;
    int thread_count = DEFAULT_THREAD_COUNT;
    int verbose = DEFAULT_VERBOSE;
    FILE *output_fp = stdout;
    FILE *log_fp = stderr;
    pthread_t threads[MAX_THREADS];
    threaddata td[MAX_THREADS];

    int opt = 0;
    while ((opt = getopt(argc, argv, "h:p:o:l:t:vH")) != -1) {
        switch (opt) {
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
                if (thread_count < 1 || thread_count > MAX_THREADS) {
                    fprintf(stderr, "Thread count must be between 1 and %d\n", MAX_THREADS);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                verbose = 1;
                break;
            case 'H':
                printf("Usage: %s -h hashfile -p passwordfile [options]\n", argv[0]);
                printf("Options:\n");
                printf("  -h filename   Input file of Argon2 hashes (required)\n");
                printf("  -p filename   Input file of plaintext passwords (required)\n");
                printf("  -o filename   Output file (default: stdout)\n");
                printf("  -l filename   Log file (default: stderr)\n");
                printf("  -t number     Number of threads to use (default: 1, max: %d)\n", MAX_THREADS);
                printf("  -v            Enable verbose logging\n");
                printf("  -H            Show this help message and exit\n");
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Usage: %s -h <hash_file> -p <password_file> -o <output_file> -l <log_file> -t <thread_count> -v\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!hash_file || !password_file) {
        fprintf(stderr, "Error: -h and -p options are both required.\n");
        exit(EXIT_FAILURE);
    }

    // Setup logging and output
    setup_logging(&log_fp, log_file, verbose);
    setup_output(&output_fp, output_file);

    // Create thread data array first
    // Load hash file into first thread's data structure
    td[0].hash_count = 0;
    td[0].hashes = create_ragged_array(hash_file, &td[0].hash_count);
    if (!td[0].hashes) {
        fprintf(log_fp, "Failed to load hash file: %s\n", hash_file);
        exit(EXIT_FAILURE);
    }

    // Load password file into first thread's data structure
    td[0].password_count = 0;
    td[0].passwords = create_ragged_array(password_file, &td[0].password_count);
    if (!td[0].passwords) {
        fprintf(log_fp, "Failed to load password file: %s\n", password_file);
        free(td[0].hashes[0]);
        free(td[0].hashes);
        exit(EXIT_FAILURE);
    }

    // Initialize thread data and share arrays
    for (int i = 0; i < thread_count; i++) {
        td[i].thread_id = i;
        td[i].hashes = td[0].hashes;         // Share the same hash array
        td[i].hash_count = td[0].hash_count;
        td[i].passwords = td[0].passwords;    // Share the same password array
        td[i].password_count = td[0].password_count;
        td[i].cracked_count = 0;
        td[i].failed_count = 0;
        td[i].output_fp = output_fp;
        td[i].log_fp = log_fp;
        td[i].verbose = verbose;

        if (pthread_create(&threads[i], NULL, thread_worker, &td[i]) != 0) {
            fprintf(log_fp, "Failed to create thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to complete
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    if (log_fp != stderr) {
        fclose(log_fp);
    }
    if (output_fp != stdout) {
        fclose(output_fp);
    }

    free(td[0].hashes[0]);
    free(td[0].hashes);
    free(td[0].passwords[0]);
    free(td[0].passwords);

    return EXIT_SUCCESS;
}
