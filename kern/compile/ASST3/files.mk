# Automatically generated by config; do not edit
console.o: ${S}/dev/generic/console.c
	${COMPILE.c} ${S}/dev/generic/console.c
SRCS+=${S}/dev/generic/console.c
OBJS+=console.o

emu.o: ${S}/dev/lamebus/emu.c
	${COMPILE.c} ${S}/dev/lamebus/emu.c
SRCS+=${S}/dev/lamebus/emu.c
OBJS+=emu.o

lser.o: ${S}/dev/lamebus/lser.c
	${COMPILE.c} ${S}/dev/lamebus/lser.c
SRCS+=${S}/dev/lamebus/lser.c
OBJS+=lser.o

lhd.o: ${S}/dev/lamebus/lhd.c
	${COMPILE.c} ${S}/dev/lamebus/lhd.c
SRCS+=${S}/dev/lamebus/lhd.c
OBJS+=lhd.o

pseudorand.o: ${S}/dev/generic/pseudorand.c
	${COMPILE.c} ${S}/dev/generic/pseudorand.c
SRCS+=${S}/dev/generic/pseudorand.c
OBJS+=pseudorand.o

random.o: ${S}/dev/generic/random.c
	${COMPILE.c} ${S}/dev/generic/random.c
SRCS+=${S}/dev/generic/random.c
OBJS+=random.o

ltimer.o: ${S}/dev/lamebus/ltimer.c
	${COMPILE.c} ${S}/dev/lamebus/ltimer.c
SRCS+=${S}/dev/lamebus/ltimer.c
OBJS+=ltimer.o

lamebus.o: ${S}/dev/lamebus/lamebus.c
	${COMPILE.c} ${S}/dev/lamebus/lamebus.c
SRCS+=${S}/dev/lamebus/lamebus.c
OBJS+=lamebus.o

lrandom.o: ${S}/dev/lamebus/lrandom.c
	${COMPILE.c} ${S}/dev/lamebus/lrandom.c
SRCS+=${S}/dev/lamebus/lrandom.c
OBJS+=lrandom.o

beep.o: ${S}/dev/generic/beep.c
	${COMPILE.c} ${S}/dev/generic/beep.c
SRCS+=${S}/dev/generic/beep.c
OBJS+=beep.o

ltrace.o: ${S}/dev/lamebus/ltrace.c
	${COMPILE.c} ${S}/dev/lamebus/ltrace.c
SRCS+=${S}/dev/lamebus/ltrace.c
OBJS+=ltrace.o

rtclock.o: ${S}/dev/generic/rtclock.c
	${COMPILE.c} ${S}/dev/generic/rtclock.c
SRCS+=${S}/dev/generic/rtclock.c
OBJS+=rtclock.o

ltimer_att.o: ${S}/dev/lamebus/ltimer_att.c
	${COMPILE.c} ${S}/dev/lamebus/ltimer_att.c
SRCS+=${S}/dev/lamebus/ltimer_att.c
OBJS+=ltimer_att.o

con_lser.o: ${S}/dev/lamebus/con_lser.c
	${COMPILE.c} ${S}/dev/lamebus/con_lser.c
SRCS+=${S}/dev/lamebus/con_lser.c
OBJS+=con_lser.o

lhd_att.o: ${S}/dev/lamebus/lhd_att.c
	${COMPILE.c} ${S}/dev/lamebus/lhd_att.c
SRCS+=${S}/dev/lamebus/lhd_att.c
OBJS+=lhd_att.o

random_lrandom.o: ${S}/dev/lamebus/random_lrandom.c
	${COMPILE.c} ${S}/dev/lamebus/random_lrandom.c
SRCS+=${S}/dev/lamebus/random_lrandom.c
OBJS+=random_lrandom.o

rtclock_ltimer.o: ${S}/dev/lamebus/rtclock_ltimer.c
	${COMPILE.c} ${S}/dev/lamebus/rtclock_ltimer.c
SRCS+=${S}/dev/lamebus/rtclock_ltimer.c
OBJS+=rtclock_ltimer.o

ltrace_att.o: ${S}/dev/lamebus/ltrace_att.c
	${COMPILE.c} ${S}/dev/lamebus/ltrace_att.c
SRCS+=${S}/dev/lamebus/ltrace_att.c
OBJS+=ltrace_att.o

lser_att.o: ${S}/dev/lamebus/lser_att.c
	${COMPILE.c} ${S}/dev/lamebus/lser_att.c
SRCS+=${S}/dev/lamebus/lser_att.c
OBJS+=lser_att.o

lrandom_att.o: ${S}/dev/lamebus/lrandom_att.c
	${COMPILE.c} ${S}/dev/lamebus/lrandom_att.c
SRCS+=${S}/dev/lamebus/lrandom_att.c
OBJS+=lrandom_att.o

emu_att.o: ${S}/dev/lamebus/emu_att.c
	${COMPILE.c} ${S}/dev/lamebus/emu_att.c
SRCS+=${S}/dev/lamebus/emu_att.c
OBJS+=emu_att.o

beep_ltimer.o: ${S}/dev/lamebus/beep_ltimer.c
	${COMPILE.c} ${S}/dev/lamebus/beep_ltimer.c
SRCS+=${S}/dev/lamebus/beep_ltimer.c
OBJS+=beep_ltimer.o

pseudorand_att.o: ${S}/dev/generic/pseudorand_att.c
	${COMPILE.c} ${S}/dev/generic/pseudorand_att.c
SRCS+=${S}/dev/generic/pseudorand_att.c
OBJS+=pseudorand_att.o

sfs_vnode.o: ${S}/fs/sfs/sfs_vnode.c
	${COMPILE.c} ${S}/fs/sfs/sfs_vnode.c
SRCS+=${S}/fs/sfs/sfs_vnode.c
OBJS+=sfs_vnode.o

sfs_fs.o: ${S}/fs/sfs/sfs_fs.c
	${COMPILE.c} ${S}/fs/sfs/sfs_fs.c
SRCS+=${S}/fs/sfs/sfs_fs.c
OBJS+=sfs_fs.o

sfs_io.o: ${S}/fs/sfs/sfs_io.c
	${COMPILE.c} ${S}/fs/sfs/sfs_io.c
SRCS+=${S}/fs/sfs/sfs_io.c
OBJS+=sfs_io.o

addrspace.o: ${S}/vm/addrspace.c
	${COMPILE.c} ${S}/vm/addrspace.c
SRCS+=${S}/vm/addrspace.c
OBJS+=addrspace.o

cache_mips1.o: ${S}/arch/mips/mips/cache_mips1.S
	${COMPILE.S} ${S}/arch/mips/mips/cache_mips1.S
SRCS+=${S}/arch/mips/mips/cache_mips1.S
OBJS+=cache_mips1.o

exception.o: ${S}/arch/mips/mips/exception.S
	${COMPILE.S} ${S}/arch/mips/mips/exception.S
SRCS+=${S}/arch/mips/mips/exception.S
OBJS+=exception.o

lamebus_mips.o: ${S}/arch/mips/mips/lamebus_mips.c
	${COMPILE.c} ${S}/arch/mips/mips/lamebus_mips.c
SRCS+=${S}/arch/mips/mips/lamebus_mips.c
OBJS+=lamebus_mips.o

interrupt.o: ${S}/arch/mips/mips/interrupt.c
	${COMPILE.c} ${S}/arch/mips/mips/interrupt.c
SRCS+=${S}/arch/mips/mips/interrupt.c
OBJS+=interrupt.o

pcb.o: ${S}/arch/mips/mips/pcb.c
	${COMPILE.c} ${S}/arch/mips/mips/pcb.c
SRCS+=${S}/arch/mips/mips/pcb.c
OBJS+=pcb.o

ram.o: ${S}/arch/mips/mips/ram.c
	${COMPILE.c} ${S}/arch/mips/mips/ram.c
SRCS+=${S}/arch/mips/mips/ram.c
OBJS+=ram.o

spl.o: ${S}/arch/mips/mips/spl.c
	${COMPILE.c} ${S}/arch/mips/mips/spl.c
SRCS+=${S}/arch/mips/mips/spl.c
OBJS+=spl.o

start.o: ${S}/arch/mips/mips/start.S
	${COMPILE.S} ${S}/arch/mips/mips/start.S
SRCS+=${S}/arch/mips/mips/start.S
OBJS+=start.o

switch.o: ${S}/arch/mips/mips/switch.S
	${COMPILE.S} ${S}/arch/mips/mips/switch.S
SRCS+=${S}/arch/mips/mips/switch.S
OBJS+=switch.o

syscall.o: ${S}/arch/mips/mips/syscall.c
	${COMPILE.c} ${S}/arch/mips/mips/syscall.c
SRCS+=${S}/arch/mips/mips/syscall.c
OBJS+=syscall.o

threadstart.o: ${S}/arch/mips/mips/threadstart.S
	${COMPILE.S} ${S}/arch/mips/mips/threadstart.S
SRCS+=${S}/arch/mips/mips/threadstart.S
OBJS+=threadstart.o

trap.o: ${S}/arch/mips/mips/trap.c
	${COMPILE.c} ${S}/arch/mips/mips/trap.c
SRCS+=${S}/arch/mips/mips/trap.c
OBJS+=trap.o

tlb_mips1.o: ${S}/arch/mips/mips/tlb_mips1.S
	${COMPILE.S} ${S}/arch/mips/mips/tlb_mips1.S
SRCS+=${S}/arch/mips/mips/tlb_mips1.S
OBJS+=tlb_mips1.o

coremap.o: ${S}/arch/mips/mips/coremap.c
	${COMPILE.c} ${S}/arch/mips/mips/coremap.c
SRCS+=${S}/arch/mips/mips/coremap.c
OBJS+=coremap.o

tlb.o: ${S}/arch/mips/mips/tlb.c
	${COMPILE.c} ${S}/arch/mips/mips/tlb.c
SRCS+=${S}/arch/mips/mips/tlb.c
OBJS+=tlb.o

vpage.o: ${S}/arch/mips/mips/vpage.c
	${COMPILE.c} ${S}/arch/mips/mips/vpage.c
SRCS+=${S}/arch/mips/mips/vpage.c
OBJS+=vpage.o

mips-setjmp.o: ${S}/../lib/libc/mips-setjmp.S
	${COMPILE.S} ${S}/../lib/libc/mips-setjmp.S
SRCS+=${S}/../lib/libc/mips-setjmp.S
OBJS+=mips-setjmp.o

copyinout.o: ${S}/lib/copyinout.c
	${COMPILE.c} ${S}/lib/copyinout.c
SRCS+=${S}/lib/copyinout.c
OBJS+=copyinout.o

notdumbvm.o: ${S}/arch/mips/mips/notdumbvm.c
	${COMPILE.c} ${S}/arch/mips/mips/notdumbvm.c
SRCS+=${S}/arch/mips/mips/notdumbvm.c
OBJS+=notdumbvm.o

array.o: ${S}/lib/array.c
	${COMPILE.c} ${S}/lib/array.c
SRCS+=${S}/lib/array.c
OBJS+=array.o

bitmap.o: ${S}/lib/bitmap.c
	${COMPILE.c} ${S}/lib/bitmap.c
SRCS+=${S}/lib/bitmap.c
OBJS+=bitmap.o

queue.o: ${S}/lib/queue.c
	${COMPILE.c} ${S}/lib/queue.c
SRCS+=${S}/lib/queue.c
OBJS+=queue.o

kheap.o: ${S}/lib/kheap.c
	${COMPILE.c} ${S}/lib/kheap.c
SRCS+=${S}/lib/kheap.c
OBJS+=kheap.o

kprintf.o: ${S}/lib/kprintf.c
	${COMPILE.c} ${S}/lib/kprintf.c
SRCS+=${S}/lib/kprintf.c
OBJS+=kprintf.o

kgets.o: ${S}/lib/kgets.c
	${COMPILE.c} ${S}/lib/kgets.c
SRCS+=${S}/lib/kgets.c
OBJS+=kgets.o

misc.o: ${S}/lib/misc.c
	${COMPILE.c} ${S}/lib/misc.c
SRCS+=${S}/lib/misc.c
OBJS+=misc.o

ntoh.o: ${S}/lib/ntoh.c
	${COMPILE.c} ${S}/lib/ntoh.c
SRCS+=${S}/lib/ntoh.c
OBJS+=ntoh.o

__printf.o: ${S}/../lib/libc/__printf.c
	${COMPILE.c} ${S}/../lib/libc/__printf.c
SRCS+=${S}/../lib/libc/__printf.c
OBJS+=__printf.o

snprintf.o: ${S}/../lib/libc/snprintf.c
	${COMPILE.c} ${S}/../lib/libc/snprintf.c
SRCS+=${S}/../lib/libc/snprintf.c
OBJS+=snprintf.o

atoi.o: ${S}/../lib/libc/atoi.c
	${COMPILE.c} ${S}/../lib/libc/atoi.c
SRCS+=${S}/../lib/libc/atoi.c
OBJS+=atoi.o

bzero.o: ${S}/../lib/libc/bzero.c
	${COMPILE.c} ${S}/../lib/libc/bzero.c
SRCS+=${S}/../lib/libc/bzero.c
OBJS+=bzero.o

memcpy.o: ${S}/../lib/libc/memcpy.c
	${COMPILE.c} ${S}/../lib/libc/memcpy.c
SRCS+=${S}/../lib/libc/memcpy.c
OBJS+=memcpy.o

memmove.o: ${S}/../lib/libc/memmove.c
	${COMPILE.c} ${S}/../lib/libc/memmove.c
SRCS+=${S}/../lib/libc/memmove.c
OBJS+=memmove.o

strcat.o: ${S}/../lib/libc/strcat.c
	${COMPILE.c} ${S}/../lib/libc/strcat.c
SRCS+=${S}/../lib/libc/strcat.c
OBJS+=strcat.o

strchr.o: ${S}/../lib/libc/strchr.c
	${COMPILE.c} ${S}/../lib/libc/strchr.c
SRCS+=${S}/../lib/libc/strchr.c
OBJS+=strchr.o

strcmp.o: ${S}/../lib/libc/strcmp.c
	${COMPILE.c} ${S}/../lib/libc/strcmp.c
SRCS+=${S}/../lib/libc/strcmp.c
OBJS+=strcmp.o

strcpy.o: ${S}/../lib/libc/strcpy.c
	${COMPILE.c} ${S}/../lib/libc/strcpy.c
SRCS+=${S}/../lib/libc/strcpy.c
OBJS+=strcpy.o

strlen.o: ${S}/../lib/libc/strlen.c
	${COMPILE.c} ${S}/../lib/libc/strlen.c
SRCS+=${S}/../lib/libc/strlen.c
OBJS+=strlen.o

strrchr.o: ${S}/../lib/libc/strrchr.c
	${COMPILE.c} ${S}/../lib/libc/strrchr.c
SRCS+=${S}/../lib/libc/strrchr.c
OBJS+=strrchr.o

strtok_r.o: ${S}/../lib/libc/strtok_r.c
	${COMPILE.c} ${S}/../lib/libc/strtok_r.c
SRCS+=${S}/../lib/libc/strtok_r.c
OBJS+=strtok_r.o

init.o: ${S}/dev/init.c
	${COMPILE.c} ${S}/dev/init.c
SRCS+=${S}/dev/init.c
OBJS+=init.o

device.o: ${S}/fs/vfs/device.c
	${COMPILE.c} ${S}/fs/vfs/device.c
SRCS+=${S}/fs/vfs/device.c
OBJS+=device.o

vfscwd.o: ${S}/fs/vfs/vfscwd.c
	${COMPILE.c} ${S}/fs/vfs/vfscwd.c
SRCS+=${S}/fs/vfs/vfscwd.c
OBJS+=vfscwd.o

vfslist.o: ${S}/fs/vfs/vfslist.c
	${COMPILE.c} ${S}/fs/vfs/vfslist.c
SRCS+=${S}/fs/vfs/vfslist.c
OBJS+=vfslist.o

vfslookup.o: ${S}/fs/vfs/vfslookup.c
	${COMPILE.c} ${S}/fs/vfs/vfslookup.c
SRCS+=${S}/fs/vfs/vfslookup.c
OBJS+=vfslookup.o

vfspath.o: ${S}/fs/vfs/vfspath.c
	${COMPILE.c} ${S}/fs/vfs/vfspath.c
SRCS+=${S}/fs/vfs/vfspath.c
OBJS+=vfspath.o

vnode.o: ${S}/fs/vfs/vnode.c
	${COMPILE.c} ${S}/fs/vfs/vnode.c
SRCS+=${S}/fs/vfs/vnode.c
OBJS+=vnode.o

devnull.o: ${S}/fs/vfs/devnull.c
	${COMPILE.c} ${S}/fs/vfs/devnull.c
SRCS+=${S}/fs/vfs/devnull.c
OBJS+=devnull.o

hardclock.o: ${S}/thread/hardclock.c
	${COMPILE.c} ${S}/thread/hardclock.c
SRCS+=${S}/thread/hardclock.c
OBJS+=hardclock.o

synch.o: ${S}/thread/synch.c
	${COMPILE.c} ${S}/thread/synch.c
SRCS+=${S}/thread/synch.c
OBJS+=synch.o

scheduler.o: ${S}/thread/scheduler.c
	${COMPILE.c} ${S}/thread/scheduler.c
SRCS+=${S}/thread/scheduler.c
OBJS+=scheduler.o

thread.o: ${S}/thread/thread.c
	${COMPILE.c} ${S}/thread/thread.c
SRCS+=${S}/thread/thread.c
OBJS+=thread.o

process.o: ${S}/proc/process.c
	${COMPILE.c} ${S}/proc/process.c
SRCS+=${S}/proc/process.c
OBJS+=process.o

main.o: ${S}/main/main.c
	${COMPILE.c} ${S}/main/main.c
SRCS+=${S}/main/main.c
OBJS+=main.o

menu.o: ${S}/main/menu.c
	${COMPILE.c} ${S}/main/menu.c
SRCS+=${S}/main/menu.c
OBJS+=menu.o

hello.o: ${S}/main/hello.c
	${COMPILE.c} ${S}/main/hello.c
SRCS+=${S}/main/hello.c
OBJS+=hello.o

sys_fork.o: ${S}/syscall/sys_fork.c
	${COMPILE.c} ${S}/syscall/sys_fork.c
SRCS+=${S}/syscall/sys_fork.c
OBJS+=sys_fork.o

sys_getpid.o: ${S}/syscall/sys_getpid.c
	${COMPILE.c} ${S}/syscall/sys_getpid.c
SRCS+=${S}/syscall/sys_getpid.c
OBJS+=sys_getpid.o

sys_waitpid.o: ${S}/syscall/sys_waitpid.c
	${COMPILE.c} ${S}/syscall/sys_waitpid.c
SRCS+=${S}/syscall/sys_waitpid.c
OBJS+=sys_waitpid.o

sys_read.o: ${S}/syscall/sys_read.c
	${COMPILE.c} ${S}/syscall/sys_read.c
SRCS+=${S}/syscall/sys_read.c
OBJS+=sys_read.o

sys_write.o: ${S}/syscall/sys_write.c
	${COMPILE.c} ${S}/syscall/sys_write.c
SRCS+=${S}/syscall/sys_write.c
OBJS+=sys_write.o

sys__exit.o: ${S}/syscall/sys__exit.c
	${COMPILE.c} ${S}/syscall/sys__exit.c
SRCS+=${S}/syscall/sys__exit.c
OBJS+=sys__exit.o

sys_execv.o: ${S}/syscall/sys_execv.c
	${COMPILE.c} ${S}/syscall/sys_execv.c
SRCS+=${S}/syscall/sys_execv.c
OBJS+=sys_execv.o

sys___time.o: ${S}/syscall/sys___time.c
	${COMPILE.c} ${S}/syscall/sys___time.c
SRCS+=${S}/syscall/sys___time.c
OBJS+=sys___time.o

sys_sbrk.o: ${S}/syscall/sys_sbrk.c
	${COMPILE.c} ${S}/syscall/sys_sbrk.c
SRCS+=${S}/syscall/sys_sbrk.c
OBJS+=sys_sbrk.o

loadelf.o: ${S}/userprog/loadelf.c
	${COMPILE.c} ${S}/userprog/loadelf.c
SRCS+=${S}/userprog/loadelf.c
OBJS+=loadelf.o

runprogram.o: ${S}/userprog/runprogram.c
	${COMPILE.c} ${S}/userprog/runprogram.c
SRCS+=${S}/userprog/runprogram.c
OBJS+=runprogram.o

uio.o: ${S}/userprog/uio.c
	${COMPILE.c} ${S}/userprog/uio.c
SRCS+=${S}/userprog/uio.c
OBJS+=uio.o

arraytest.o: ${S}/test/arraytest.c
	${COMPILE.c} ${S}/test/arraytest.c
SRCS+=${S}/test/arraytest.c
OBJS+=arraytest.o

bitmaptest.o: ${S}/test/bitmaptest.c
	${COMPILE.c} ${S}/test/bitmaptest.c
SRCS+=${S}/test/bitmaptest.c
OBJS+=bitmaptest.o

queuetest.o: ${S}/test/queuetest.c
	${COMPILE.c} ${S}/test/queuetest.c
SRCS+=${S}/test/queuetest.c
OBJS+=queuetest.o

threadtest.o: ${S}/test/threadtest.c
	${COMPILE.c} ${S}/test/threadtest.c
SRCS+=${S}/test/threadtest.c
OBJS+=threadtest.o

tt3.o: ${S}/test/tt3.c
	${COMPILE.c} ${S}/test/tt3.c
SRCS+=${S}/test/tt3.c
OBJS+=tt3.o

synchtest.o: ${S}/test/synchtest.c
	${COMPILE.c} ${S}/test/synchtest.c
SRCS+=${S}/test/synchtest.c
OBJS+=synchtest.o

malloctest.o: ${S}/test/malloctest.c
	${COMPILE.c} ${S}/test/malloctest.c
SRCS+=${S}/test/malloctest.c
OBJS+=malloctest.o

fstest.o: ${S}/test/fstest.c
	${COMPILE.c} ${S}/test/fstest.c
SRCS+=${S}/test/fstest.c
OBJS+=fstest.o

autoconf.o: ${S}/compile/ASST3/autoconf.c
	${COMPILE.c} ${S}/compile/ASST3/autoconf.c
SRCS+=${S}/compile/ASST3/autoconf.c
OBJS+=autoconf.o

