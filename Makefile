.PHONY: all raspi arduino test

all: raspi arduino

raspi:
	$(MAKE) -C libraries/ucomm
	$(MAKE) -C raspi-uart

arduino:
	$(MAKE) -C arduino

test:
	$(MAKE) -C libraries/ucomm test
