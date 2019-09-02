SRC_DIR=.
SRCS=$(shell find $(SRC_DIR) -name '*.cpp')
OBJS=$(subst .cpp,.o,$(SRCS))
TARGET=test.exe
CPPFLAGS=-O1
CC=g++

.PHONY: default all tidy clean

default: all

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

tidy:
	rm -f $(OBJS)

clean: tidy
	rm -f $(TARGET)
