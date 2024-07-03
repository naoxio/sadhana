CC = tcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -L$(LIBRARY_PATH) -ltermbox
INCLUDES = -I$(C_INCLUDE_PATH) -I$(SRCDIR)

SRCDIR = src
BINDIR = bin
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = $(BINDIR)/sadhana

all: $(TARGET)

$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -f $(OBJS) $(TARGET)
	rmdir $(BINDIR) 2>/dev/null || true

.PHONY: all clean