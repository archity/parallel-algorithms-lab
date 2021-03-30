CC = mpicc
CFLAGS =
LDFLAGS =

EXEC = 	hello_world.run					\
	hello_world_even_rank.run			\
	simple_communication.run			\
	simple_communication_ping_pong.run 	\
	communication_ring.run				\
	blocking_communication.run 			\
	nonblocking_communication.run		\

HEADER_FILES = $(wildcard *.h)


all: $(EXEC)

%.run: %.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c $(HEADER_FILES)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXEC) *.o *~

.PHONY: clean
