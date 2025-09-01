# project1_group16

# Multithreaded File Copier – Subtask 1

## Overview

This program copies a file using multiple threads:

- **Readers** read lines from the source file and put them into a shared queue (max 20 lines).
- **Writers** take lines from the queue and write them to the destination file.

Subtask 1 only: no locks or condition variables yet → race conditions and unfinished writers may occur.

---

## Usage

1. **Generate input file**

```bash
chmod +x generate_text.sh
./generate_text.sh 30 > input.txt
```

2. **Compile**

```bash
    gcc -o mscopier mscopier.c -lpthread
```

2. **Run**

```bash
    ./mscopier n input.txt output.txt
```

- n = number of reader/writer threads (2–10)

# Example

```bash
./generate_text.sh 10 > input.txt
./mscopier 3 input.txt output.txt
```
