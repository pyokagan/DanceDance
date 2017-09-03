objects := $(patsubst %.c, %.o, $(wildcard ../libraries/ucomm/src/*.c) $(wildcard *.c)) \
	$(patsubst %.cpp, %.o, $(wildcard ../libraries/ucomm/src/*.cpp) $(wildcard *.cpp))

raspi-monitor: $(objects)

%.o: %.c
	$(CC) -MMD $(CPPFLAGS) $(CFLAGS) -I../libraries/ucomm/src -c -o $@ $<

-include $(objects:.o=.d)
