CFLAGS = -g -MMD -Wall
objects := $(patsubst %.c, %.o, $(wildcard src/*.c)) \
	$(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

.PHONY: clean

libucomm.a: $(objects)
	$(RM) $@ && ar rcs $@ $^

clean:
	$(RM) $(objects) $(objects:.o=.d) libucomm.a

-include $(objects:.o=.d)
