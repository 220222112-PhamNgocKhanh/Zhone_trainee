CC = gcc

TARGETS = target_app watchdog_daemon

all: $(TARGETS)

target_app: target_app.c
	$(CC)  target_app.c -o target_app

watchdog_daemon: watchdog_daemon.c
	$(CC)  watchdog_daemon.c -o watchdog_daemon

clean:
	rm -f $(TARGETS) /my_watchdog_shm

.PHONY: all clean

