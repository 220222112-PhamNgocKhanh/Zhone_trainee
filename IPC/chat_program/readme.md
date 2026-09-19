# Lab: Two-Way Chat Program using FIFO (Named Pipe)

This lab demonstrates **inter-process communication (IPC)** between two independent processes using FIFOs (named pipes) and POSIX threads in Linux.

## Idea

Two separate client processes (`client1` and `client2`) communicate with each other in full-duplex mode:

* A single FIFO is unidirectional, so **two FIFOs** are used:
  * `/tmp/chat_fifo_1_to_2`: transfers data from Client 1 to Client 2.
  * `/tmp/chat_fifo_2_to_1`: transfers data from Client 2 to Client 1.
* **Deadlock avoidance**: By default, `open()` on a FIFO blocks until the other end is opened:
  * Client 1 opens `FIFO_1_TO_2` for writing, then `FIFO_2_TO_1` for reading.
  * Client 2 opens `FIFO_1_TO_2` for reading, then `FIFO_2_TO_1` for writing.
  * This complementary open order ensures neither process gets stuck waiting for the other.
* **Full-duplex communication**: Each client uses a dedicated receiver thread (`pthread`) to continuously listen for incoming messages, while the main thread waits for user input from `stdin`.
* Typing `exit` or `quit` disconnects the client and removes the FIFOs from the filesystem.

## Implementation

* `client1.c` implements the first client process.
* `client2.c` implements the second client process.
* `mkfifo()` creates the two named pipes with permissions `0666`.
* `receive_messages()` runs in a separate thread, continuously calling `read()` on the incoming FIFO:
  * Prints messages sent by the peer to the terminal.
  * When `read()` returns `0` (EOF), detects that the peer has disconnected and exits.
* The `main()` thread uses `fgets()` to read user input from `stdin` and `write()` to send it over the outgoing FIFO.
* `unlink()` removes the temporary FIFO files when a client terminates.

## Compilation and Execution

Compile both clients using `make`:

```bash
make
```

Open two separate terminal windows and run each client:

**Terminal 1:**
```bash
./client1
```

**Terminal 2:**
```bash
./client2
```

Either client can start first without causing a deadlock. Type messages in either terminal to chat in real time. 

To clean up build artifacts and temporary FIFOs:

```bash
make clean
```

## Output on My Machine

### Terminal 1 (`client1`)

```text
[Client 1] Waiting for Client 2 to connect...
[Client 1] Connected! Type your message (type 'exit' to quit):
Client 1: Hello from Client 1!

Client 2: Hi Client 1, how are you?
Client 1: Everything is working great via FIFO!

Client 2: Awesome! I will exit now.
Client 1: 
```

### Terminal 2 (`client2`)

```text
[Client 2] Waiting for Client 1 to connect...
[Client 2] Connected! Type your message (type 'exit' to quit):
Client 2: 

Client 1: Hello from Client 1!
Client 2: Hi Client 1, how are you?

Client 1: Everything is working great via FIFO!
Client 2: Awesome! I will exit now.
```

This output shows bidirectional real-time communication: both clients can send and receive messages concurrently, and disconnection of one client is detected gracefully by the other.

