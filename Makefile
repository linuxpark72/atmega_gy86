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

load: clean $(PROG).bin 
	$(LOADER) -p $(MCU_TARGET) -c $(ISP_PROG_TYPE) -P $(ISP_DEV_NAME) -U flash:w:$(PROG).bin:r

tag:
	ctags *

lst:  $(PROG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

text: bin

bin:  $(PROG).bin

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
