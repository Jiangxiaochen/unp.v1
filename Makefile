CFLAGS := -std=gnu99 -Wall -g

.PHONY: clean all

C_FILES := $(shell ls *.c)
H_FILES := $(shell ls *.h)
TARGETS := $(addprefix bin/, $(basename $(C_FILES)))

all: $(TARGETS)

./bin/%:%.c $(H_FILES)
	gcc $(CFLAGS) $< -o $@ -lunp

clean:
	rm -f ./bin/*

