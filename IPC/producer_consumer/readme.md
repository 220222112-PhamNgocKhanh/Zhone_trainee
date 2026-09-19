# Lab: Producer - Consumer Problem using System V Shared Memory and Semaphores

This lab demonstrates **inter-process communication (IPC)** and **process synchronization** in Linux using **System V Shared Memory** and **System V Semaphore**.

---

## 1. Problem Idea

The Producer - Consumer problem is implemented using a single producer and three concurrent consumers:

* **Shared Resource (Shared Memory):**
  * An integer variable `number_of_stock` is stored in a shared memory segment representing the current inventory level.
* **Synchronization (System V Semaphore Set):**
  * A single semaphore set containing **2 semaphores** manages synchronization:
    1. **Semaphore 0 (`sem_number_client`):** A **counting semaphore** initialized to `2`. It limits concurrency so that at most **2 consumers** can access and process items at the same time. If a 3rd consumer attempts to enter, it is blocked until one of the active consumers finishes its turn.
    2. **Semaphore 1 (`sem_lock`):** A **binary semaphore (mutex)** initialized to `1`. It enforces mutual exclusion when accessing the shared variable `number_of_stock`. Only one process (either the producer or one consumer) can read or modify the stock at any time, preventing **race conditions**.
* **Producer Behavior:**
  * Initializes the stock to `10`.
  * Periodically (every 10 seconds), it locks the mutex, checks the current stock, and refills it to `10` if it has dropped below `10`.
  * Catches `SIGINT` (Ctrl+C) using a signal handler to automatically destroy the shared memory segment and semaphore set.
* **Consumer Behavior (`consumer1`, `consumer2`, `consumer3`):**
  * Three independent consumer processes run concurrently.
  * In each loop iteration, a consumer:
    1. Acquires a client slot (decrements Semaphore 0).
    2. Acquires the mutex lock (decrements Semaphore 1).
    3. If `number_of_stock > 1`, it consumes 2 items (`*number_of_stock -= 2`).
    4. If `number_of_stock <= 1`, it reports "out of stock".
    5. Releases the mutex lock (increments Semaphore 1).
    6. Releases the client slot (increments Semaphore 0).
    7. Sleeps for 3 seconds before the next attempt.

---

## 2. Program Architecture and How It Is Built

### Key Generation and Shared File Anchor
* Both shared memory and semaphores use `ftok()` with file path `"/tmp/temp"`.
* The producer creates `"/tmp/temp"` using `open(STOCK, O_CREAT | O_RDWR, 0666)` to ensure the anchor file exists.
* Separate project IDs are assigned to differentiate IPC objects:
  * `project_id_shm = 21` for the shared memory segment.
  * `project_id_sem = 2` for the semaphore set.

### Producer Architecture (`producer.c`)
1. **Signal Handling:**
   * Registers `handler_sigint` for `SIGINT` via `sigaction()`.
   * When terminated with `Ctrl+C`, the handler removes the shared memory segment (`shmctl(..., IPC_RMID)`) and the semaphore set (`semctl(..., IPC_RMID)`), preventing orphaned IPC resources in the system.
2. **IPC Allocation:**
   * Allocates shared memory with `shmget(key, 4, 0666 | IPC_CREAT)`.
   * Allocates a 2-element semaphore set with `semget(key, 2, 0666 | IPC_CREAT)`.
3. **Semaphore Initialization:**
   * Sets Semaphore 0 to `2` (concurrency limit).
   * Sets Semaphore 1 to `1` (mutex lock).
4. **Shared Memory Attachment:**
   * Maps the segment via `shmat()`.
   * Sets initial `*number_of_stock = 10`.
5. **Production Loop:**
   * Sleeps 10 seconds.
   * Decrements Semaphore 1 to lock the mutex.
   * Checks and refills stock to 10 if necessary.
   * Increments Semaphore 1 to unlock the mutex.

### Consumer Architecture (`consumer1.c`, `consumer2.c`, `consumer3.c`)
1. **Connection:**
   * Obtains identifiers using `shmget()` and `semget()` without `IPC_CREAT`.
   * Validates return values; if the producer is not running, prints an informative error and exits.
2. **One-Time Memory Attachment:**
   * Calls `shmat()` once before the `while (1)` loop to avoid memory leaks.
3. **Synchronization Sequence (LIFO Order):**
   * P(Sem 0) $\rightarrow$ acquire client slot.
   * P(Sem 1) $\rightarrow$ acquire mutex lock.
   * Access critical section: read and update `*number_of_stock`.
   * V(Sem 1) $\rightarrow$ release mutex lock.
   * V(Sem 0) $\rightarrow$ release client slot.
   * Sleeps for 3 seconds.

---

## 3. Purpose of Each File

| File | Purpose |
| :--- | :--- |
| **`producer.c`** | Creates and initializes the shared memory and semaphore set. Continuously checks and refills stock. Catches `SIGINT` to safely remove IPC resources. |
| **`consumer1.c`** | Consumer process #1. Connects to the existing IPC objects and consumes 2 items per cycle. |
| **`consumer2.c`** | Consumer process #2. Runs concurrently with Consumer 1 and Consumer 3. |
| **`consumer3.c`** | Consumer process #3. Demonstrates the concurrency limit (one consumer waits when 2 are already processing). |
| **`Makefile`** | Build automation file for compiling all binaries (`producer`, `consumer1`, `consumer2`, `consumer3`) and cleaning up (`clean`). |

---

## 4. Compilation, Execution, and Testing

### Step 1: Compile All Programs
Run `make` to compile the executables:
```bash
make clean && make
```

### Step 2: Test Error Handling
Attempt to run a consumer before starting the producer:
```bash
./consumer1
```
**Expected Output:** The process safely catches the error and exits:
```text
shmget failed (producer might not be running): No such file or directory
```

### Step 3: Start the Producer
Open **Terminal 1** and start the producer:
```bash
./producer
```

### Step 4: Start the Consumers
Open three additional terminals and launch each consumer:

* **Terminal 2:**
  ```bash
  ./consumer1
  ```
* **Terminal 3:**
  ```bash
  ./consumer2
  ```
* **Terminal 4:**
  ```bash
  ./consumer3
  ```

### Step 5: Test Resource Cleanup
In Terminal 1 (producer), press `Ctrl + C`:
```text
^C
```
The `handler_sigint` intercepts the signal and immediately removes the shared memory segment and semaphore set.
Verify with `ipcs` that no resources remain allocated:
```bash
ipcs -s -m
```

---

## 5. Output on my device

### Producer Output
```text
Producer started, initial stock = 10
Producer: refilled stock to 10
```

### Consumer1
```t
Consumer 1 started
Consumer 1 sucessed, remaining: 8
Consumer 1 sucessed, remaining: 2
Consumer 1: out of stock
Consumer 1: out of stock
Consumer 1 sucessed, remaining: 6
Consumer 1 sucessed, remaining: 0
```

### Consumer2
```text
Consumer 2 started
Consumer 2 sucessed, remaining: 6
Consumer 2 sucessed, remaining: 0
Consumer 2: out of stock
Consumer 2: out of stock
Consumer 2 sucessed, remaining: 4
Consumer 2: out of stock
Consumer 2: out of stock
```

### Consumer3

```t
Consumer 3 started
Consumer 3 sucessed, remaining: 4
Consumer 3: out of stock
Consumer 3: out of stock
Consumer 3 sucessed, remaining: 8
Consumer 3 sucessed, remaining: 2
Consumer 3: out of stock
```
