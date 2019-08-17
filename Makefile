###
# Folders
###
LIB_DIR := lib
SRC_DIR := src
BUILD_DIR := build

#####
# Files
#####

HEADERS := $(shell find $(SRC_DIR) -name '*.h')
SOURCES := $(shell find $(SRC_DIR) -name '*.c')

LIB_HEADERS := $(shell find $(LIB_DIR) -name '*.h')
LIB_SOURCES := $(shell find $(LIB_DIR) -name '*.c')

#####
# Compiler Settings
#####
FLAGS := -g -Wall -O1 -mmcu=atmega328p -D F_CPU=8000000UL -I$(SRC_DIR) -I$(LIB_DIR) -Wl,--gc-sections -ffunction-sections -fdata-sections

#####
# Build Program
#####

$(BUILD_DIR)/prgm.hex: $(BUILD_DIR)/prgm.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

$(BUILD_DIR)/prgm.elf: $(LIB_HEADERS) $(LIB_SOURCES) $(HEADERS) $(SOURCES) 
	avr-gcc $(FLAGS) -o $@ $(LIB_SOURCES) $(SOURCES)

#####
# Cleanup
#####
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	
#####
# Communication Functions
#####

.PHONY: program
program: $(BUILD_DIR)/prgm.hex
	avrdude -v -c usbtiny -p m328p -U flash:w:$(BUILD_DIR)/prgm.hex
	
.PHONY: fuses
fuses:
	avrdude -v -c usbtiny -p m328p -U efuse:w:0xFF:m -U hfuse:w:0xD9:m -U lfuse:w:0xE2:m

.PHONY: serial
serial:
	screen /dev/cu.usbserial-A5XK3RJT 38400
