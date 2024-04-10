UNAME := $(shell uname)
ARCH := $(shell uname -m)
MAKE = make
CC = gcc
CXX = g++
CFLAGS = -Wall -O2 -g -pthread
CXXFLAGS = $(CFLAGS)
RM = rm -rf
ASM = nasm
ifeq ($(UNAME), Darwin)
ASMFLAGS = -f macho64 -O0 -g --prefix _
LDFLAGS = -pthread
ifeq ($(ARCH), arm64)
X86_ON_M1=arch -x86_64
endif
else
ASMFLAGS = -f elf64 -O0 -g -F dwarf
LDFLAGS = -pthread
endif

# other implicit rules
%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $< 

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o : %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

default: laplace_serial laplace_parallel

all: laplace_serial laplace_parallel

laplace_serial: laplace_serial.o matrix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ laplace_serial.o matrix.o

laplace_parallel: laplace_parallel.o matrix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ laplace_parallel.o matrix.o

clean:
	$(RM) *.o laplace_serial laplace_parallel

depend:
	$(CC) -MM $(CFLAGS) laplace_serial.c laplace_parallel.c matrix.c > Makefile.dep

-include Makefile.dep
