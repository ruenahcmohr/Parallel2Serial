

SRC = $(wildcard *.c) $(wildcard *.S)
OBJS = $(SRC:%.c=%.o)
HEADERS = $(SRC:%.c=%.h)

PRG            = main
OBJ            = program.o 
MCU_TARGET     = atmega8
OPTIMIZE       = -O2

# DEFS           = -W1,-u,vprintf -lprintf_flt -lm
DEFS           =
LIBS           = 

ASRC = i2cmaster.S

# You should not have to change anything below here.

CC             = avr-gcc
CXX             = avr-g++

# Override is only needed by avr-lib build system.

# override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -I/usr/local/avr/include
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -I/usr/local/avr/avr/include
override CXXFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map 

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text eeprom size

$(PRG).elf: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PRG).elf $(SRC) $(LIBS)

size: ${PRG}.elf
	avr-size -C --mcu=${MCU_TARGET} ${PRG}.elf

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

install: 
	avrdude -p m8 -e -U flash:w:$(PRG).hex

usbinstall:
	avrdude -c avrisp2 -P usb -p m8 -e -U flash:w:$(PRG).hex    

aspinstall:
	avrdude -c usbasp -P usb -B 40khz -p m8 -e -U flash:w:$(PRG).hex

usbfuseext:
	avrdude -c avrisp2 -P usb -p m8 -U lfuse:w:0xfe:m -U hfuse:w:0xd1:m 

aspfuseext:
	avrdude -c usbasp -P usb -p m8 -U lfuse:w:0xfe:m -U hfuse:w:0xd1:m 

usbfuseintfast:
	avrdude -c avrisp2 -P usb -p m8 -U lfuse:w:0xA4:m -U hfuse:w:0xD9:m 

aspfuseintfast:
	avrdude -c usbasp -P usb -B 40khz -p m8 -U lfuse:w:0xA4:m -U hfuse:w:0xD9:m

usbfuseint:
	avrdude -c avrisp2 -P usb -p m8 -U lfuse:w:0xe1:m -U hfuse:w:0xd1:m 

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec


