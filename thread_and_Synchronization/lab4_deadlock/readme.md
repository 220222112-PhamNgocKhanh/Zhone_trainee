# Lab 4: Deadlock

This lab demonstrates how a **deadlock** can occur when multiple threads
lock shared resources in different orders.

## Overview

* **`deadlock.c`**: Thread A locks `lock1` then waits for `lock2`, while
  Thread B locks `lock2` then waits for `lock1`. Both threads wait forever.
* **`deadlock_lock_ordering.c`**: Fixes the problem by making both threads
  acquire `lock1` before `lock2`.

## Compilation and Execution

### 1. Deadlock example

```bash
gcc -o deadlock deadlock.c
./deadlock
```

The program may stop without finishing because both threads are waiting for
each other.

### 2. Fixed version

```bash
gcc -o deadlock_lock_ordering deadlock_lock_ordering.c
./deadlock_lock_ordering
```

## Fix


With lock ordering, each mutex is assigned a priority or ordering level. When a thread needs to acquire multiple mutexes, it must always acquire them in the same order, from the highest to the lowest priority.

For example, if lock1 has a higher priority than lock2, both threads must acquire lock1 first and lock2 second. By consistently following the same lock order across all threads, we can prevent circular waiting and avoid deadlocks.
