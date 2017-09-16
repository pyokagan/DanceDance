CFLAGS=-g -I../libraries/ucomm/src -Wall -MMD
LDFLAGS=-pthread
objects := $(patsubst %.c, %.o,$(wildcard *.c))

.PHONY: clean ../libraries/ucomm/libucomm.a

raspi-uart: $(objects) ../libraries/ucomm/libucomm.a

../libraries/ucomm/libucomm.a:
	$(MAKE) -C ../libraries/ucomm

clean:
	$(RM) $(objects) $(objects:.o=.d) raspi-uart
	$(MAKE) -C ../libraries/ucomm clean

-include $(objects:.o=.d)
