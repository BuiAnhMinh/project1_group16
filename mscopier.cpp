#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_QUEUE 20
#define MAX_LINE 1024

// Shared queue
char *queue[MAX_QUEUE];
int front = 0, rear = 0, count = 0;

// File pointers
FILE *src, *dst;

// Number of threads
int n;

// Simple enqueue (no locks yet)
void enqueue(char *line)
{
    if (count == MAX_QUEUE)
    {
        // Queue full – in subtask 2/3 we’ll block here
        return;
    }
    queue[rear] = strdup(line);
    rear = (rear + 1) % MAX_QUEUE;
    count++;
}

// Simple dequeue (no locks yet)
char *dequeue()
{
    if (count == 0)
    {
        // Queue empty – in subtask 2/3 we’ll block here
        return NULL;
    }
    char *line = queue[front];
    front = (front + 1) % MAX_QUEUE;
    count--;
    return line;
}

// Reader thread
void *reader_thread(void *arg)
{
    char line[MAX_LINE];
    while (1)
    {
        // Critical: read from source file
        if (fgets(line, sizeof(line), src) == NULL)
        {
            break; // End of file
        }
        enqueue(line);
    }
    return NULL;
}

// Writer thread
void *writer_thread(void *arg)
{
    while (1)
    {
        char *line = dequeue();
        if (line == NULL)
        {
            // nothing in queue yet, just continue
            continue;
        }
        fprintf(dst, "%s", line);
        free(line);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // Arg Logic
    // Checking if there are enough arguments
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s n source_file destination_file\n", argv[0]);
        exit(1);
    }
    // Checking if the number of threads is between 2 and 10
    n = atoi(argv[1]);
    if (n < 2 || n > 10)
    {
        fprintf(stderr, "n must be between 2 and 10\n");
        exit(1);
    }
    //  Checking to see if the source file exists
    src = fopen(argv[2], "r");
    if (!src)
    {
        perror("Error opening source file");
        exit(1);
    }
    // Checking output file
    dst = fopen(argv[3], "w");
    if (!dst)
    {
        perror("Error opening destination file");
        fclose(src);
        exit(1);
    }

    pthread_t readers[n], writers[n];

    // Create threads
    for (int i = 0; i < n; i++)
    {
        pthread_create(&readers[i], NULL, reader_thread, NULL);
        pthread_create(&writers[i], NULL, writer_thread, NULL);
    }

    // Join reader threads
    for (int i = 0; i < n; i++)
    {
        pthread_join(readers[i], NULL);
    }

    // Note: Writers will spin forever in Subtask 1
    // (In Subtask 2/3 we’ll add proper signaling to stop them)

    fclose(src);
    fclose(dst);
    return 0;
}
