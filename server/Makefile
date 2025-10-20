CC=gcc
CFLAGS=-O2 -Wall -Wextra -I../libs
LDFLAGS=

BIN=weather_server
OBJS=main.o ../libs/tcp.o ../libs/http.o

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

../libs/%.o: ../libs/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(BIN) *.o ../libs/*.o