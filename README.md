# project1_group16

## **Worklog** 
- **Task 1:** @Spudgee
- **Task 2:**
    - **Subtask 1:** @KevinChen2211
    - **Subtask 2:** @SatisfyingCrunch
    - **Subtask 3:** @BuiAnhMinh

All team members collaborated on the following:
- Initial approach planning 
- Error-handling 
- Fixing bugs
- Refining approach/cleaning code
- Confirming final code adhered to the spec requirements
- Testing compilation on RMIT servers (tested on Titan, Jupiter, Saturn)

---
## **Locks and Condition Variables**

### Mutex Declarations
- **Line 17:** `pthread_mutex_t file_mutex;`
- **Line 18:** `pthread_mutex_t write_mutex;`
- **Line 19:** `pthread_mutex_t queue_mutex;`

### Condition Variable Declarations
- **Line 20:** `pthread_cond_t not_full;`
- **Line 21:** `pthread_cond_t not_empty;`

### Mutex Initializations
- **Lines 127-132:** All three mutexes initialized with error checking

### Condition Variable Initializations
- **Lines 133-136:** Both condition variables initialized with error checking

### Mutex Usage
- **Lines 27, 38, 59, 67, 72, 80, 87, 92:** Lock/unlock operations for `queue_mutex`, `file_mutex`, and `write_mutex`

### Condition Variable Usage
- **Line 31:** `pthread_cond_wait(&not_full, &queue_mutex);`
- **Line 35:** `pthread_cond_signal(&not_empty);`
- **Line 48:** `pthread_cond_wait(&not_empty, &queue_mutex);`
- **Line 53:** `pthread_cond_signal(&not_full);`

---

## **Generate input file**

```bash
chmod +x generate_text.sh
./generate_text.sh 30 > input.txt
```
This creates a text file `input.txt` with 30 random lines from `task1/wordlist.10000`

---

## **Compilation**

### Build All Files
```bash
    make
```
or
```bash
    make all
```

### Build Single File
```bash
make mmcopier
make mscopier
```

### Clean Build Files
```bash
make clean
```

### Delete Existing Build and Rebuild
```bash
make clean && make
```

---

## Usage

## Multi-Threaded Multi-File Copying (Task 1)

This program copies multiple files using multiple threads 

- **Readers** read files concurrently with each thread responsible for reading a single file in source directory
- **Witers** write files concurrently with each thread responsible for writing a single file to the destination directory

### Run
```bash
    ./mmcopier n <source_dir> <destination_dir>
```

- `n` = number of reader/writer threads (2–10)
- `<source_dir>`- input directory to copy files of format `source{n}.txt` where `n ∈ [1, n]`
- `<destination_dir>` - output directory to copy files into (must already exist)

## Multi-Threaded Single-File Copying (Task 2)

This program copies a file using multiple threads:

- **Readers** read lines from the source file and put them into a shared queue (max 20 lines).
- **Writers** take lines from the queue and write them to the destination file.

### Run 

```bash
    ./mscopier n <input.txt> <output.txt>
```

- `n` = number of reader/writer threads (2–10)
- `<input.txt>`- input file to read lines from
- `<output.txt>` - output file to write lines into

## Example

```bash
./generate_text.sh 10 > input.txt

./mcopier 6 task1/source_dir task2/dest_dir

./mscopier 3 input.txt output.txt
```
