
CC      := gcc
TARGET  := robot-server
SRCDIR  := src
OBJDIR  := build

SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

CFLAGS  := -Wall -Wextra -O2 -g -I$(SRCDIR)
LDFLAGS := -lmicrohttpd -lpthread

.PHONY: all clean run install-service

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	@echo ""
	@echo "  Build OK -> ./$(TARGET)"
	@echo "  Run with: make run   (or ./$(TARGET) from this directory)"
	@echo ""

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: all
	./$(TARGET)
