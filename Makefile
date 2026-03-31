
CC      := gcc
TARGET  := robot-server
SRCDIR  := src
LIBDIR  := lib
OBJDIR  := build

SRCS    := $(wildcard $(SRCDIR)/*.c)   
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

CFLAGS  := -Wall -Wextra -O2 -g -I$(SRCDIR)

# LDFLAGS para la compilación REAL en la Raspberry Pi (incluye pigpio)
LDFLAGS_REAL := -lmicrohttpd -lpthread -lmpg123 -lasound -lpigpio -lrt

# LDFLAGS para la compilación de SIMULACIÓN en PC (excluye pigpio)
LDFLAGS_SIM  := -lmicrohttpd -lpthread -lmpg123 -lasound


.PHONY: all clean run sim run-sim

# --- COMPILACIÓN PARA RASPBERRY PI ---
all: LDFLAGS = $(LDFLAGS_REAL)
all: $(OBJDIR) $(TARGET)

# --- COMPILACIÓN PARA SIMULACIÓN EN PC ---
sim: CFLAGS += -DSIMULACION
sim: LDFLAGS = $(LDFLAGS_SIM)
sim: clean $(OBJDIR) $(TARGET)

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

run-sim: sim
	@mkdir -p audio
	./$(TARGET)



