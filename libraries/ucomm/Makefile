CFLAGS = -g -MMD -Wall -I.
objects := $(patsubst %.c, %.o, $(wildcard src/*.c)) \
	$(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

.PHONY: clean test

libucomm.a: $(objects)
	$(RM) $@ && ar rcs $@ $^

test: test/hello
	test/hello

test/hello: libucomm.a

clean:
	$(RM) $(objects) $(objects:.o=.d) libucomm.a

-include $(objects:.o=.d)
