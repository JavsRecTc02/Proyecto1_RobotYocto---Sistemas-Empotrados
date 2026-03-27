
CC      := gcc
TARGET  := robot-server
SRCDIR  := src
LIBDIR  := lib
OBJDIR  := build

SRCS    := $(wildcard $(SRCDIR)/*.c)   
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

CFLAGS  := -Wall -Wextra -O2 -g -I$(SRCDIR)
LDFLAGS := -lmicrohttpd -lpthread -lmpg123 -lasound


.PHONY: all clean run install-service

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: all
	@mkdir -p audio
	./$(TARGET)
