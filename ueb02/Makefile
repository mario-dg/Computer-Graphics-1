# Quelldateien
SRCS             = main.c logic.c scene.c io.c stringOutput.c

# ausfuehrbares Ziel
TARGET           = ueb02

# Objektdateien
OBJS             = $(SRCS:.c=.o)

# Compiler
CC               = gcc

# Linker Flags
CFLAGS  = -Wall -Wextra -Wno-unused-parameter -Werror

# Linker
LD               = gcc

# Linker libraries
LDLIBS    	 = -lm  -lglut -lGLU -lGL 

.SUFFIXES: .o .c
.PHONY: all clean

# TARGETS
all: $(TARGET)

# Linken des ausfuehrbaren Programms
$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDLIBS) -o $(TARGET)

# Kompilieren der Objektdateien
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $*.o $*.c

# einfaches Aufraeumen
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f *~

