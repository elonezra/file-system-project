file = main
CFLAGS = -Wall -Wextra
D ?= 0

ifeq ($(D), 1)
    CFLAGS += -DDEBUG -g
endif

all: main

main: $(file).c
	gcc $(CFLAGS) $(file).c -o main

clean:
	rm -f main

run: clean main
	./main ../my_files/