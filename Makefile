CC=gcc

TARGET=libmyasan.so
CFLAGS=-fPIC

OBJS=asan.o

.PHONY: all, clean

all: $(TARGET)

asan.o: asan.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^

clean:
	rm -f $(TARGET)
	rm -f *.o