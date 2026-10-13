# Chat Room Server

TCP chat server written in C, using epoll edge-triggered with non-blocking sockets and a thread pool of 4 workers.

## Design

### Thread model

The server separates connection acceptance from I/O processing. The main thread runs a blocking `accept()` loop — nothing else. When a new connection arrives, it sets the socket to non-blocking and pushes the fd to one of four worker threads in round-robin order. From that point on, the main thread forgets about that connection entirely.

Each worker manages its own set of connections independently. Workers do not share work or communicate with each other.

This separation exists for a simple reason: `accept()` is cheap and fast, while reading, parsing, and broadcasting can take variable time depending on how many clients are connected. Mixing both in one thread means a slow broadcast blocks new clients from being accepted. Separating them keeps the accept path always responsive.

### Why each worker has its own epoll instance

The naive approach is one shared epoll fd that all threads call `epoll_wait()` on. This runs into the thundering herd problem — when a fd becomes readable, the kernel wakes up all threads waiting on that epoll, but only one will actually `accept()` or `recv()` the data. The rest return empty-handed and go back to sleep. This creates unnecessary context switching under load.

With one epoll per worker, only the worker that owns a given fd will ever see events from it. There is no contention on `epoll_wait()`, and no wasted wakeups.

### Why edge-triggered (EPOLLET)

Level-triggered epoll reports an fd as ready on every `epoll_wait()` call as long as data remains in the buffer. Edge-triggered only fires once, when new data arrives. ET produces fewer epoll_wait wakeups because the kernel does not need to re-check the fd state on every poll cycle.

The trade-off is that with ET, you must read the entire receive buffer in a single shot every time you get an event. If you leave data unread, the event will not fire again until more data arrives — you will miss what is already there. This is why `drain_fd()` loops on `recv()` until it gets `EAGAIN`, which means the kernel buffer is empty.

Non-blocking sockets are mandatory here. If the socket were blocking and the buffer happened to be empty at the wrong moment, `recv()` would stall the worker indefinitely. With `O_NONBLOCK`, `recv()` returns `EAGAIN` immediately when there is nothing left to read, letting `drain_fd()` exit cleanly.

### Why a ring buffer to pass fds from main to workers

When a new client connects, the main thread needs to tell a worker "take this fd." The simplest option is a condition variable — main signals, worker wakes up, takes the fd. But this means the main thread blocks until the worker is ready to receive, which creates unnecessary coupling between the two.

A ring buffer decouples them. Main writes the fd to the buffer and continues accepting the next connection without waiting. The worker reads from the buffer on its own schedule, at the start of each loop iteration. Neither thread blocks on the other.

A ring buffer also has predictable memory usage and zero dynamic allocation. There is no heap involved, which keeps things fast and avoids fragmentation. The trade-off is a fixed capacity — if more than 256 fds are queued before a worker processes them (which should not happen in normal operation), the oldest entries get silently overwritten. This is acceptable for the scale this server targets.

### Client table indexed by fd

Client state is stored in `clients[MAX_CLIENTS]`, a flat array indexed directly by fd value. When you need to look up the username or authentication status of fd 42, you read `clients[42]` — no hash, no search.

This works because Linux guarantees that fd values are non-negative integers starting from a small number (typically 3 after stdin/stdout/stderr). The kernel also reuses the lowest available fd when a connection closes, so the array stays dense in practice.

### Flow overview

```
main thread
  accept() -> set O_NONBLOCK -> push fd into worker_queues[rr % 4]

worker thread (x4)
  drain queue -> epoll_ctl(ADD, fd, EPOLLIN|EPOLLET)
  epoll_wait(50ms timeout)
  EPOLLIN  -> drain_fd() -> handle_auth() or handle_message() -> broadcast()
  EPOLLERR/EPOLLHUP -> client_remove()
```

The 50ms timeout on `epoll_wait` exists so workers periodically check their queue even when no I/O events are happening. Without a timeout, a worker sitting idle would not notice new fds pushed by the main thread until another event happened to wake it.

### Authentication

There is no real authentication. The first message a client sends is taken as its username. After that, every subsequent message is formatted as `[username] text` and broadcast to all other connected clients. This keeps the protocol simple — plain text over a raw TCP socket, newline delimited.

## Files

```
server.h    structs, constants, shared prototypes
server.c    all logic
Makefile
```

## Build

```bash
make
```

Requires gcc and pthreads (standard on Linux).

## Run

```bash
./chat_server
```

Listens on port 5000 by default. Change `SERVER_PORT` in `server.h` to use a different port.

## Test

Open multiple terminals and connect with netcat:

```bash
nc 127.0.0.1 5000
```

Each connection will be prompted for a username. After entering a name, anything you type is broadcast to all other connected clients.

To stop the server: `Ctrl+C` triggers a graceful shutdown — all worker threads are joined before exit.

## Notes

- `MAX_CLIENTS` is set to 1024 and is also the upper bound on fd values the server will accept.
- `broadcast()` is best-effort. If a write to one client fails, it does not affect delivery to others.
- `SIGPIPE` is ignored globally. Without this, writing to a closed socket would kill the process.
