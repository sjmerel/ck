ifeq ($(shell uname -m),x86_64)
  ARCH := x64
else
  ARCH := x86
endif

hellocricket: hellocricket.cpp hellocricket.ckb Makefile
	g++ hellocricket.cpp \
    -o hellocricket \
    -I../../../../inc \
    -L../../../../lib/linux/$(ARCH)/debug \
    -L/usr/lib/`uname -m`-linux-gnu \
    -lck -lasound -lpthread

hellocricket.ckb: ../hellocricket.ckbx
	../../../../bin/linux/$(ARCH)/cktool buildbank ../hellocricket.ckbx hellocricket.ckb

clean: 
	rm hellocricket
.PHONY: clean

