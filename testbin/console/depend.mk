
console.o: \
 console.c \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/err.h

