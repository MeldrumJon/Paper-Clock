###
# Folders
###
ARDUINO_DIR := arduino-core
LIB_DIR := lib
SRC_DIR := src
BUILD_DIR := build

#####
# Files
#####

HEADERS := $(shell find $(SRC_DIR) -name '*.h')
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp' -o -name '*.c')

CORE_HEADERS := $(shell find $(ARDUINO_DIR) -name '*.h')
CORE_SOURCES := $(shell find $(ARDUINO_DIR) -name '*.cpp' -o -name '*.c')

LIB_HEADERS := $(shell find $(LIB_DIR) -name '*.h')
LIB_SOURCES := $(shell find $(LIB_DIR) -name '*.cpp' -o -name '*.c')

# Library include paths for the compiler
LIB_PATHS := $(shell ls -d -1 "$(LIB_DIR)/"**/)
LIB_PARAMS := $(foreach d, $(LIB_PATHS), -I$d)

#####
# Compiler Settings
#####
#-fno-threadsafe-statics 
FLAGS = -mmcu=atmega328p 
FLAGS += -DF_CPU=8000000UL -DARDUINO=10809
FLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
FLAGS += -I$(ARDUINO_DIR) -I$(SRC_DIR) $(LIB_PARAMS)
FLAGS += -Os -Wl,--gc-sections -ffunction-sections -fdata-sections
FLAGS += -fno-exceptions -Wno-error=narrowing
FLAGS += -fno-fat-lto-objects -flto -fuse-linker-plugin
#####
# Build Program
#####

$(BUILD_DIR)/prgm.hex: $(HEADERS) $(SOURCES) $(CORE_HEADERS) $(CORE_SOURCES) $(LIB_HEADERS) $(LIB_SOURCES)
	mkdir -p $(BUILD_DIR)
	avr-gcc $(FLAGS) -o $(BUILD_DIR)/prgm.elf $(SOURCES) $(LIB_SOURCES) $(CORE_SOURCES)
	avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)/prgm.elf $(BUILD_DIR)/prgm.hex


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
	avrdude -v -c usbtiny -p m328p -B1 -U flash:w:$(BUILD_DIR)/prgm.hex
	
.PHONY: fuses
fuses:
	avrdude -v -c usbtiny -p m328p -B5 -U efuse:w:0xFF:m -U hfuse:w:0xD9:m -U lfuse:w:0xE2:m

.PHONY: serial
serial:
	screen /dev/cu.usbserial-A5XK3RJT 38400
