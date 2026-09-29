# UDP Echo Server and Connected Client

A simple UDP Echo Server and Client implementation in C.

This project demonstrates the use of **Connected UDP**. The server handles UDP datagrams iteratively using `recvfrom` and `sendto`. The client, however, uses `connect()` on its UDP socket. 

Using a connected UDP socket in the client has several advantages:
1. It allows the use of `send()` and `recv()` (or `write()` and `read()`) instead of `sendto()` and `recvfrom()`.
2. It allows the application to receive asynchronous errors. For example, if the server is not running and the client sends a packet, the ICMP "Port Unreachable" error will be returned to the client as an `ECONNREFUSED` error on a subsequent socket operation.
3. It can be more efficient if the client sends multiple packets to the same destination, as the OS kernel does not need to perform route lookups and connect/disconnect the socket on every packet.

## Build

Compile the server and client using `make`:

```bash
make
```

To clean up build artifacts:

```bash
make clean
```

## Run the Server

Start the server (listens on default port `1234`):

```bash
./server
```

You can also specify a custom port:

```bash
./server <port>
```

## Run the Client

In a separate terminal, start the client, specifying the server IP address (and optionally the port):

```bash
./client 127.0.0.1
```

Or with a custom port:

```bash
./client 127.0.0.1 <port>
```

Type any text in the client terminal and press **Enter** to see the echoed message from the server. Press `Ctrl+D` to disconnect.
