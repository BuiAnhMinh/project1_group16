#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#define MAX_QUEUE 20
#define MAX_LINE 1024

// Shared queue
char *queue[MAX_QUEUE];
int front = 0, rear = 0, count = 0;

// File pointers
FILE *src, *dst;

// Number of threads
pthread_mutex_t file_mutex;
pthread_mutex_t write_mutex;
pthread_mutex_t queue_mutex;
pthread_cond_t not_full;
pthread_cond_t not_empty;

// Add terminiation logic globals
int active_readers = 0; // counter of reader that haven't reached end of file yet
int writers_total = 0; 

// Simple enqueue (no locks yet)
void enqueue(char *line) {
    pthread_mutex_lock(&queue_mutex);
    while (count == MAX_QUEUE) {
        // Wait for consumer to remove items from queue
        printf("QUEUE FULL\n");
        pthread_cond_wait(&not_full, &queue_mutex);
    }
    queue[rear] = line ? strdup(line) : NULL; // poison pill allowed
    rear = (rear + 1) % MAX_QUEUE;
    count++;
    pthread_cond_signal(&not_empty);

    if (line)
        printf("[Thread %llu] ADDED TO QUEUE: %s", (unsigned long long)pthread_self(), line);
    else
        printf("[Thread %llu] ADDED POISON PILL\n", (unsigned long long)pthread_self());

    pthread_mutex_unlock(&queue_mutex);
}

// Simple dequeue (no locks yet)
char *dequeue() {
    pthread_mutex_lock(&queue_mutex);
    while (count == 0) {
        // Wait for producer to add items to queue
        printf("QUEUE EMPTY\n");
        pthread_cond_wait(&not_empty, &queue_mutex);
    }
    
    char *line = queue[front];
    front = (front + 1) % MAX_QUEUE;
    count--;
    pthread_cond_signal(&not_full);

    if (line)
        printf("[Thread %llu] READ %s", (unsigned long long)pthread_self(), line);
    else
        printf("[Thread %llu] RECEIVED POISON PILL\n", (unsigned long long)pthread_self());

    pthread_mutex_unlock(&queue_mutex);
    return line; // this can be NULL (posion pill)
}

// Reader thread
void *reader_thread(void *arg) {
    // char line[MAX_LINE];
    // while (1) {
    //     // Critical: read from source file
    //     pthread_mutex_lock(&file_mutex);
    //     if (fgets(line, sizeof(line), src) == NULL) {
    //         // Add termination logic here
    //         pthread_mutex_unlock(&file_mutex);
    //         break; // End of file
    //     }
    //     enqueue(line);
    //     pthread_mutex_unlock(&file_mutex);
    // }
    // return NULL;
    char line [MAX_LINE];
    for (;;){
        pthread_mutex_lock(&file_mutex);
        char *got = fgets(line, sizeof(line), src);
        if (!got){
            //this reader is finished
            active_readers--;
            int remaining = active_readers;
            pthread_mutex_unlock(&file_mutex);

            if (remaining == 0){
                // last reader : send N poison pills
                for (int i = 0; i < writers_total; i++){
                    enqueue(NULL);
                }
            }
            break;
        }
        pthread_mutex_unlock(&file_mutex);
        enqueue(line);
    }
    return NULL;
}

// Writer thread
void *writer_thread(void *arg) {
    // pthread_mutex_lock(&write_mutex);
    // while (1) {
    //     char *line = dequeue();
    //     if (line == NULL) {
    //         // nothing in queue yet, just continue
    //         continue;
    //     }
    //     fprintf(dst, "%s", line);
    //     free(line);
    // }
    // pthread_mutex_unlock(&write_mutex);
    // return NULL;

    for (;;){
        char *line = dequeue();
        if (line == NULL) {
            break;
        }
        pthread_mutex_lock(&write_mutex);
        fputs(line, dst);
        pthread_mutex_unlock(&write_mutex);
        free(line);
    }
    return NULL; 
}

int main(int argc, char *argv[]) {
    // Arg Logic
    // Checking if there are enough arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s n source_file destination_file\n", argv[0]);
        exit(1);
    }
    // Checking if the number of threads is between 2 and 10
    int n = atoi(argv[1]);
    if (n < 2 || n > 10) {
        fprintf(stderr, "n must be between 2 and 10\n");
        exit(1);
    }
    //  Checking to see if the source file exists
    src = fopen(argv[2], "r");
    if (!src) {
        perror("Error opening source file");
        exit(1);
    }
    // Checking output file
    dst = fopen(argv[3], "w");
    if (!dst) {
        perror("Error opening destination file");
        fclose(src);
        exit(1);
    }

    pthread_mutex_init(&file_mutex, NULL);
    pthread_mutex_init(&write_mutex, NULL);
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    active_readers = n;
    writers_total = n;

    std::vector<pthread_t> readers(n);
    std::vector<pthread_t> writers(n);

    // Create threads
    for (int i = 0; i < n; i++) {
        pthread_create(&readers[i], NULL, reader_thread, NULL);
        pthread_create(&writers[i], NULL, writer_thread, NULL);
    }

    // Join reader threads
    for (int i = 0; i < n; i++) {
        pthread_join(readers[i], NULL);
    }

    // Join writer threads
    for (int i = 0; i < n; i++) {
        pthread_join(writers[i], NULL);
    }

    // Note: Writers will spin forever in Subtask 1
    // (In Subtask 2/3 we’ll add proper signaling to stop them)
    pthread_mutex_destroy(&file_mutex);
    pthread_mutex_destroy(&write_mutex);
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    fclose(src);
    fclose(dst);
    return 0;
}
