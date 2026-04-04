CC      := gcc
TARGET  := robot-server
SRCDIR  := src
LIBDIR  := lib
OBJDIR  := build

# Directorios
SRCS    := $(wildcard $(SRCDIR)/*.c) $(wildcard $(LIBDIR)/*.c)

# Objetos 
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/src/%.o, \
           $(filter $(SRCDIR)/%.c, $(SRCS))) \
           $(patsubst $(LIBDIR)/%.c, $(OBJDIR)/lib/%.o, \
           $(filter $(LIBDIR)/%.c, $(SRCS)))

CFLAGS  := -Wall -Wextra -O2 -g -I$(SRCDIR) -Ilib

LDFLAGS := -lmicrohttpd -lpthread -lmpg123 -lasound

.PHONY: all clean run

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)/src $(OBJDIR)/lib

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJDIR)/src/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/lib/%.o: $(LIBDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: all
	@mkdir -p audio
	./$(TARGET)