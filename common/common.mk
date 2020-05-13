CC=sdcc
CFLAGS=-mmcs51
# needed for newer 89C52, see https://github.com/grigorig/stcgal/issues/50
ARCH=stc12
INCLUDES=-I../include
LIBS=
PORT=/dev/ttyUSB0

%.asm:		%.c
		$(CC) $(CFLAGS) $(INCLUDES) -S $<

%.rel:		%.c
		$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $(<:.c=.rel)

%.ihx:		%.rel
		$(CC) $(CFLAGS) -o $@ $<

%.ibn:		%.ihx
		hex2bin -e ibn $<

%.flash:	%.ihx
ifndef ARCH
		stcgal -p $(PORT) $<
else
		stcgal -P $(ARCH) -p $(PORT) $<
endif

clean:
		rm -f *.{asm,sym,lst,rel,rst,sym,lk,map,mem,ihx,ibn}
