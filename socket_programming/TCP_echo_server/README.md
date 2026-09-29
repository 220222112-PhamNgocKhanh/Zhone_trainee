# TCP Echo Server

A simple TCP Echo Server in C using the **fork-per-connection** model. For every new client connection, the server forks a child process to handle reading from the socket and echoing the exact data back to the client.

## Build

Compile the server using `make`:

```bash
make
```

To clean up build artifacts:

```bash
make clean
```

## Run

Start the server (listens on default port `1234`):

```bash
./server
```

You can also specify a custom port:

```bash
./server <port>
```

## Test

Test the echo server using `nc` (netcat) or `telnet`:

```bash
nc (ip_of_server) 1234
```

Type any text and press **Enter** to see the echoed message. Press `Ctrl+C` to disconnect.

