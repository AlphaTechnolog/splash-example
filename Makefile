CC := clang

LIBS := raylib
CFLAGS := -Wall -Werror $(shell pkg-config --cflags $(LIBS)) -O3
LDFLAGS := $(shell pkg-config --libs $(LIBS))

SRCS := $(shell find . -type f -name "*.c")
OBJS := $(SRCS:.c=.o)

TARGET := app

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

run: $(TARGET)
	./$<

clean: $(TARGET) $(OBJS)
	rm $^
