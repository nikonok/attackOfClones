GCC=gcc
TARGET=main.c
OBJECTS=main.o

default: build

%.o: %.c
	$(GCC) $< -o $@
	
build: $(OBJECTS)

clean:
	rm -rf log* $(TARGET)
