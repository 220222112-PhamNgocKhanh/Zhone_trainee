

# Lab 1: Race Condition Identification and Mitigation using Mutex

This lab demonstrates the occurrence of a **race condition** in multithreaded applications and how to resolve it using POSIX threads (`pthreads`) and mutexes.

## Overview

* **`race_condition.c`**: Demonstrates a classic race condition where two concurrent threads attempt to increment a shared global counter simultaneously without synchronization.
* **`race_condition_fix.c`**: Implements mutual exclusion (`pthread_mutex_t`) to ensure thread-safe access to the shared resource, guaranteeing a deterministic and correct final output.

---

## Getting Started


### Compilation and Execution

#### 1. Demonstrating the Race Condition

Compile and run the vulnerable program:

```bash
gcc -o race_condition race_condition.c 
./race_condition
```

> **Note:** Due to concurrent, uncoordinated writes to the shared variable, the final counter value will often be less than the expected `200000`, varying across executions.

#### 2. Applying the Mutex Fix

Compile and run the synchronized program:

```bash
gcc -o race_condition_fix race_condition_fix.c 
./race_condition_fix
```

> **Result:** By enforcing mutual exclusion, the mutex ensures that only one thread modifies the shared counter at a time. The final output will consistently and correctly equal `200000`.

---

## Implementation Details

* **Race Condition (`race_condition.c`)**: Two threads run concurrently, each incrementing the global variable $100,000$ times. Because the increment operation (read-modify-write) is not atomic, context switches lead to lost updates.
* **Mutex Synchronization (`race_condition_fix.c`)**: A `pthread_mutex_t` lock is acquired before modifying the shared variable and released immediately afterward, serializing critical sections and preventing data corruption.

