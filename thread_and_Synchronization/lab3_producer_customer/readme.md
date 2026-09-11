# Lab 3: Producer-Consumer with a Bounded Queue

This lab demonstrates the **producer-consumer problem** using POSIX threads,
a mutex, and condition variables.

## Idea

Three producer threads create jobs and add them to a shared queue. One printer
thread consumes the jobs and processes them.

The queue has a maximum size of 5:

* If the queue is full, producers wait.
* If the queue is empty, the printer waits.
* The mutex protects the queue and `queue_count` from concurrent access.
* Condition variables notify threads when the queue becomes available.

The queue data structure used in this lab was implemented by me during the
queue practice in `C_programming_review`.

## Implementation

* `producer_customer.c` creates the producer and printer threads.
* `queue.c` and `queue.h` provide the `push()` and `pop()` operations.
* `doubly_link_list.c` and `doubly_link_list.h` provide the linked-list
  implementation used by the queue.
* `queue_mutex` protects shared queue data.
* `queue_not_empty` wakes the printer when a producer adds a job.
* `queue_not_full` wakes producers when the printer removes a job.

Each producer creates 5 jobs, so the total number of jobs is 15. Job IDs are
generated from the producer ID, for example `100`, `101`, and `200`.

## Compilation and Execution

Compile the program with:

```bash
make
./producer_customer
```

The program keeps the printer thread running and waiting for new jobs after
all producers finish. Press `Ctrl+C` to stop it.

## Output on My Machine

The output order can change between runs because the threads run concurrently.
One run produced:

```text
Printer: queue is empty, waiting...
Producer 2: added job 200 (queue = 1)
Producer 1: added job 100 (queue = 2)
Producer 3: added job 300 (queue = 3)
Printer: processing job 200 (queue = 2)
Producer 2: added job 201 (queue = 3)
Producer 1: added job 101 (queue = 4)
Producer 3: added job 301 (queue = 5)
Producer 2: queue is full, waiting...
Producer 1: queue is full, waiting...
Producer 3: queue is full, waiting...
Printer: processing job 100 (queue = 4)
Producer 2: added job 202 (queue = 5)
Producer 2: queue is full, waiting...
Printer: processing job 300 (queue = 4)
Producer 1: added job 102 (queue = 5)
Producer 1: queue is full, waiting...
Printer: processing job 201 (queue = 4)
Producer 3: added job 302 (queue = 5)
Producer 3: queue is full, waiting...
Printer: processing job 101 (queue = 4)
Producer 2: added job 203 (queue = 5)
Producer 2: queue is full, waiting...
Printer: processing job 301 (queue = 4)
Producer 1: added job 103 (queue = 5)
Producer 1: queue is full, waiting...
Printer: processing job 202 (queue = 4)
Producer 3: added job 303 (queue = 5)
Producer 3: queue is full, waiting...
Printer: processing job 102 (queue = 4)
Producer 2: added job 204 (queue = 5)
Printer: processing job 302 (queue = 4)
Producer 1: added job 104 (queue = 5)
Printer: processing job 203 (queue = 4)
Producer 3: added job 304 (queue = 5)
Printer: processing job 103 (queue = 4)
Printer: processing job 303 (queue = 3)
Printer: processing job 204 (queue = 2)
Printer: processing job 104 (queue = 1)
Printer: processing job 304 (queue = 0)
Printer: queue is empty, waiting...
```

This output shows both important cases: producers wait when the queue reaches
5, and the printer waits when the queue becomes empty.
