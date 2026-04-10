# --- Makefile compatible con Yocto Project / Cross-Compilation ---

# Yocto inyectará su propio CC (ej. arm-poky-linux-gnueabi-gcc). 
# El ?= permite usar el de Yocto si existe, o usar gcc.
CC      ?= gcc
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

# Se usa += para no sobreescribir las banderas de sysroot de Yocto
CFLAGS  += -Wall -Wextra -O2 -g -I$(SRCDIR) -I$(LIBDIR) -I/usr/local/include

# LDFLAGS con += para preservar la ruta del linker de la imagen de Yocto
LDFLAGS += -L/usr/local/lib -lmicrohttpd -lpthread -lmpg123 -lasound -lpigpio -lrt

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
	sudo ./$(TARGET)