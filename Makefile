#
# file    : Makefile 
#
# Desc    : 
# Created : 2017. 12. 10
# Author  : jeho park<linuxpark@gmail.com>
#
include	./Makefile.tmpl

all: $(PROG).elf lst text

$(PROG).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o  $@  $^  $(LIBS)

clean:
	rm -rf *.o $(PROG).elf *.eps *.png *.pdf *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

load: $(PROG).bin
	$(LOADER) -p $(MCU_TARGET) -c $(ISP_PROG_TYPE) -P $(ISP_DEV_NAME) -U flash:w:$(PROG).bin:r

lst:  $(PROG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

text: hex bin srec

hex:  $(PROG).hex
bin:  $(PROG).bin
srec: $(PROG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
