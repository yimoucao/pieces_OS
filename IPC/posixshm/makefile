########################################
#
#  IUPUI CSCI 503 -- Operating Systems
#  Yimou Cao
########################################

## Compiler, tools and options
CC      = gcc
LINK    = gcc
OPT     = -g

CCFLAGS = $(OPT) -Wall
LDFLAGS = $(OPT)

## Libraries
LIBS = -lrt
INC  = -I.

## FILES
OBJECTS = main.o utils.o ringbuffer.o
TARGET  = main


## Implicit rules
.SUFFIXES: .c
.c.o:
	$(CC) -c $(CCFLAGS) $(INC) $<

## Build rules
all: $(TARGET) 

$(TARGET): $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET) 
	rm -f *~ core