#
# SCCS ID: %W%	%G%
#
# Makefile to control the compiling, assembling and linking of
# standalone programs in the DSL.  Used for both 4003-406 and
# 4003-506 (with appropriate tweaking).
#

#
# User supplied files
#
U_C_SRC = clock.c klibc.c processes.c queues.c scheduler.c \
	sio.c stacks.c syscalls.c system.c ulibc.c user.c  \
	ramdisk.c
U_C_OBJ = clock.o klibc.o processes.o queues.o scheduler.o \
	sio.o stacks.o syscalls.o system.o ulibc.o user.o  \
	ramdisk.o
U_S_SRC = klibs.S ulibs.S
U_S_OBJ = klibs.o ulibs.o
U_LIBS	=


#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP2_CONFIG		enable SP2-specific startup variations
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG

#
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!!
#
# Compilation/assembly control
#

INCLUDE_OPT = -I
EXTINCLUDES = -I-

#
# We only want to include from the current directory and ~wrc/include
#
INCLUDES = $(EXTINCLUDES) $(INCLUDE_OPT) . $(INCLUDE_OPT) /home/fac/wrc/include

#
# Compilation/assembly/linking commands and options
#
CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc $(INCLUDES)

CC = gcc
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS) -std=c99

AS = gas
ASFLAGS =

LD = gld

NM = gnm

#		
# Transformation rules - these ensure that all compilation
# flags that are necessary are specified
#
# Note use of 'cpp' to convert .S files to temporary .s files: this allows
# use of #include/#define/#ifdef statements. However, the line numbers of
# error messages reflect the .s file rather than the original .S file. 
# (If the .s file already exists before a .S file is assembled, then
# the temporary .s file is not deleted.  This is useful for figuring
# out the line numbers of error messages, but take care not to accidentally
# start fixing things by editing the .s file.)
#

.SUFFIXES:	.S .b

.c.s:
	$(CC) $(CFLAGS) -S $*.c

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) -o $*.o $*.s -a=$*.lst
	$(RM) -f $*.s

# .s.b:
# 	$(AS) -o $*.o $*.s -a=$*.lst
# 	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

# Binary/source file for system bootstrap code

BOOT_OBJ = bootstrap.b
BOOT_SRC = bootstrap.S

# Assembly language object/source files

S_OBJ = startup.o isr_stubs.o $(U_S_OBJ)
S_SRC =	startup.S isr_stubs.S $(U_S_SRC)

# C object/source files

C_OBJ =	c_io.o support.o $(U_C_OBJ)
C_SRC =	c_io.c support.c $(U_C_SRC)

# Collections of files

OBJECTS = $(S_OBJ) $(C_OBJ)

SOURCES = $(BOOT_SRC) $(S_SRC) $(C_SRC)

#
# Targets for remaking bootable image of the program
#

all:
	if [ `uname -s` = "Linux" ]; then	\
		$(MAKE)				\
			CC="gcc -m32"		\
			AS="as --32"		\
			LD="ld -melf_i386"	\
			NM="nm"			\
			INCLUDE_OPT="-iquote"	\
			EXTINCLUDES=""		\
			floppy.image		;\
	else					\
		$(MAKE)	usb.image		;\
	fi

usb.image: bootstrap.b prog.b prog.nl BuildImage Offsets #prog.dis 
	./BuildImage -d usb -o usb.image -b bootstrap.b prog.b 0x10000

floppy.image: bootstrap.b prog.b prog.nl BuildImage Offsets #prog.dis 
	./BuildImage -d floppy -o floppy.image -b bootstrap.b prog.b 0x10000

prog.out: $(OBJECTS)
	$(LD) -o prog.out $(OBJECTS)

prog.o:	$(OBJECTS)
	$(LD) -o prog.o -Ttext 0x10000 $(OBJECTS) $(U_LIBS)

bootstrap.b:	bootstrap.o
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o bootstrap.b bootstrap.o

prog.b:	prog.o
	$(LD) -o prog.b -s --oformat binary -Ttext 0x10000 prog.o

#
# Targets for copying bootable image onto boot devices
#

floppy:	floppy.image
	dd if=floppy.image of=/dev/fd0

usbu:	usb.image
	dd if=usb.image of=/local/usbu

usbl:	usb.image
	dd if=usb.image of=/local/usbl

#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

BuildImage:	BuildImage.c
	$(CC) -o BuildImage BuildImage.c

Offsets:	Offsets.c processes.h queues.h
	$(CC) $(INCLUDES) -fno-builtin-fork -o Offsets Offsets.c

#
# Clean out this directory
#

clean:
	rm -f *.nl *.lst *.b *.o *.s *.image *.dis BuildImage Offsets

#
# Create a printable namelist from the prog.o file
#

prog.nl: prog.o
	$(NM) -Bng prog.o | pr -w80 -3 > prog.nl

#
# Generate a disassembly
#

prog.dis: prog.o
	dis prog.o > prog.dis

#
#       makedepend is a program which creates dependency lists by
#       looking at the #include lines in the source files
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

bootstrap.o: bootstrap.h
startup.o: bootstrap.h
isr_stubs.o: bootstrap.h
ulibs.o: syscalls.h headers.h x86arch.h
c_io.o: c_io.h startup.h support.h 
support.o: startup.h support.h c_io.h 
support.o: bootstrap.h
clock.o: headers.h clock.h processes.h queues.h stacks.h scheduler.h
clock.o: startup.h uart.h
clock.o: 
klibc.o: headers.h
processes.o: headers.h processes.h queues.h stacks.h
queues.o: headers.h queues.h processes.h stacks.h
scheduler.o: headers.h scheduler.h processes.h queues.h stacks.h
sio.o: headers.h sio.h queues.h processes.h stacks.h scheduler.h startup.h
sio.o: uart.h 
stacks.o: headers.h stacks.h queues.h
syscalls.o: headers.h syscalls.h  bootstrap.h
syscalls.o: clock.h processes.h queues.h stacks.h scheduler.h sio.h startup.h
syscalls.o: ulib.h user.h
system.o: headers.h ulib.h processes.h queues.h stacks.h system.h types.h
system.o: clock.h scheduler.h sio.h syscalls.h
system.o:  user.h bootstrap.h startup.h
system.o: support.h
ulibc.o: headers.h
user.o: headers.h user.h processes.h queues.h stacks.h
