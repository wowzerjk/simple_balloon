KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD     := $(shell pwd)

obj-m   += balloon.o

default:
		$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules

install:
	   $(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules_install

clean:
	   $(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) clean
