# Discovering MPI

## Hello World!

Output after executing `mpirun -n 4 ./hello_world.run`:

```
Hello, I'm Process 0/4
Hello, I'm Process 1/4
Hello, I'm Process 2/4
Hello, I'm Process 3/4
```

## Action based on the rank

Output:

```
Even process, rank: 0/4
Hello, I'm Process 1/4
Even process, rank: 2/4
Hello, I'm Process 3/4
```

## Distributed Execution


## Simple Communication

Output:

```
Rank 1 recived this from rank 0:
0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
```

## Ping-Pong

Output:
```
R0 received this back from R1:
0, 1, 4, 9, 16, 25, 36, 49, 64, 81,

R1 received this from R0:
0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
Modifications sent back to R0
```

We see that the ordering of the messages is a bit different than expected. Anything after `MPI_Recv()` is processed only after the process has received something from the sender.

Even after this, all the messages from process R0 (aka the `printf` statements) because `\n` flushes the buffer in a `printf` statement. Absence of `\n` might delay the printing of the message and we might get a different order of message.


## Communication Ring

```
mpirun -n 4 ./communication_ring.run

R0: 0
Modified to: 83

R1:
Received: 83
Modified to: 166

R2:
Received: 166
Modified to: 249

R3:
Received: 249
Modified to: 332


R0 Received back: 332

```

## Blocking/Non-Blocking Communication

### Blocking

In this situation, 2 processes are trying to send messages to each other at the same time. First we try to tackle the problem with blocking communication primitives `MPI_Send()` and `MPI_Receive()`.

Output:

```

R1 Received: Hello from P0!

[HP-Laptop:1365] *** An error occurred in MPI_Recv
[HP-Laptop:1365] *** reported by process [1086455809,0]
[HP-Laptop:1365] *** on communicator MPI_COMM_WORLD
[HP-Laptop:1365] *** MPI_ERR_RANK: invalid rank
[HP-Laptop:1365] *** MPI_ERRORS_ARE_FATAL (processes in this communicator will now abort,
[HP-Laptop:1365] ***    and potentially your MPI job)
[warn] Epoll ADD(4) on fd 31 failed.  Old events were 0; read change was 0 (none); write change was 1 (add): Bad file descriptor
```

We see that blocking communication fails when when both processes try to send at the same time.

### Non-Blocking

