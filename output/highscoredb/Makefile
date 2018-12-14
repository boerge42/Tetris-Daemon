SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
TARGET = highscoredb
CC := gcc
CFLAGS = -Wall
CFLAGS += -Os
CLIBS  += -lmosquitto -lsqlite3

$(TARGET): $(patsubst %.c, %.o, $(SRC))
	$(CC) $(CFLAGS) -o $(TARGET) $^ $(CLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	strip $(TARGET)
	cp $(TARGET) /usr/local/bin/
	chown root:root /usr/local/bin/$(TARGET)
	chmod 0755 /usr/local/bin/$(TARGET)
	
clean:
	rm -f *.o *~ $(TARGET) Makefile.dep

Makefile.dep: $(SRC) $(HDR)
	$(CC) -MM *.c > Makefile.dep

-include Makefile.dep
