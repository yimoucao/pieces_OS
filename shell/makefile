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
LIBS = 
INC  = -I.

## FILES
OBJECTS = main.o parser.o executor.o builtins.o
TARGET  = main


## Implicit rules
.SUFFIXES: .c
.c.o:
	$(CC) -c $(CCFLAGS) $(INC) $<

## Build rules
all: $(TARGET)

debug: OPT += -DDEBUG
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET) 
	rm -f *~ core