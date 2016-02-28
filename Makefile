obj-m += mis.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	cp mis.ko /lib/modules/`uname -r`/
	depmod -a



ubuntu-install: all
	cp mis.ko /lib/modules/`uname -r`/
	depmod -a
	echo "mis" >> /etc/modules

