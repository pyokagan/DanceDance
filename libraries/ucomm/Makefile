CFLAGS = -g -MMD -Wall -I.
LDFLAGS = -lm
objects := $(patsubst %.c, %.o, $(wildcard src/*.c)) \
	$(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

.PHONY: clean test

libucomm.a: $(objects)
	$(RM) $@ && ar rcs $@ $^

test: test/hello test/SampleAssembler
	test/hello
	test/SampleAssembler

test/hello: libucomm.a

test/SampleAssembler: test/SampleAssembler.o libucomm.a

clean:
	$(RM) $(objects) $(objects:.o=.d) libucomm.a

-include $(objects:.o=.d)
