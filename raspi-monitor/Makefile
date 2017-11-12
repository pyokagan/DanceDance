CFLAGS=-g -I../libraries/ucomm/src -Wall -MMD
objects := $(patsubst %.c, %.o,$(wildcard *.c))

.PHONY: clean ../libraries/ucomm/libucomm.a

raspi-monitor: $(objects) ../libraries/ucomm/libucomm.a

../libraries/ucomm/libucomm.a:
	$(MAKE) -C ../libraries/ucomm

clean:
	$(RM) $(objects) $(objects:.o=.d) raspi-monitor
	$(MAKE) -C ../libraries/ucomm clean

-include $(objects:.o=.d)
