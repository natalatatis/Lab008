CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pthread
TARGET=scheduler
OBJDIR=obj

SRCS=main.c utils.c fifo.c rr.c sjf.c srtf.c
OBJS=$(SRCS:%.c=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Esta regla crea la carpeta obj antes de compilar los archivos .o.
# Asi los archivos objeto no quedan mezclados con el codigo fuente.
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Esta regla compila cualquier archivo .c y guarda su .o dentro de obj/.
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
