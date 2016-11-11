# software version
SW_VER = 1.0.0

# hardware version
HW_VER = 1.0

# output name
TARGET = cc-cnc

# linker script
LD_SCRIPT = stm32l152cc.ld

# sources
SRC += main.c startup.c

# sys sources
SRC += ./sys/src/sem.c ./sys/src/ev.c 

# util soutrces
SRC += ./util/src/stdio.c ./util/src/string.c
SRC += ./util/src/aessw.c

# dev sources
SRC += ./dev/src/systime.c ./dev/src/watchdog.c
SRC += ./dev/src/usart1.c ./dev/src/debug.c
SRC += ./dev/src/power.c ./dev/src/extimux.c
SRC += ./dev/src/usb.c ./dev/src/usbdesc.c
SRC += ./dev/src/usbvcp.c ./dev/src/cpuclock.c  
SRC += ./dev/src/led.c ./dev/src/spi1.c
SRC += ./dev/src/sr.c ./dev/src/drv8824.c
SRC += ./dev/src/i2c1.c ./dev/src/mcp4728.c
SRC += ./dev/src/tbase.c ./dev/src/pwm.c
SRC += ./dev/src/mcp23008.c ./dev/src/tsense.c

# cnc stuff
SRC += ./cnc/src/mchn.c ./cnc/src/motors.c
SRC += ./cnc/src/cm.c ./cnc/src/gcode.c
SRC += ./cnc/src/limits.c ./cnc/src/spindle.c

# test sources
SRC += ./test/src/led.c

# input processing
SRC += ./input/src/input.c

# compiler flags
CC_FLAGS  = -mcpu=cortex-m3 -mthumb -g -Os -I. -fdata-sections 
CC_FLAGS += -ffunction-sections -Wl,--gc-sections 
CC_FLAGS += -Wall -Wmissing-field-initializers -Wsign-compare
CC_FLAGS += -Wmissing-parameter-type -Wold-style-declaration
CC_FLAGS += -Woverride-init -Wtype-limits -Wuninitialized
CC_FLAGS += -Wcast-align -Wno-format
CC_FLAGS += -pedantic-errors -std=c99
# version information
CC_FLAGS += -DSW_VER=\"$(SW_VER)\" -DHW_VER=\"$(HW_VER)\" 
 
# linker flags 
LD_FLAGS  = -T$(LD_SCRIPT)
LD_FLAGS += -nostartfiles -Wl,-Map=$(TARGET).map,--cref

# object copy flags 
OBC_FLAGS = -O binary

# tools
CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBC = arm-none-eabi-objcopy
OBD = arm-none-eabi-objdump
NM = arm-none-eabi-nm
RM = rm
SIZE = arm-none-eabi-size
ECHO = echo
CHKSUM = chksum

# sources converted to objs
OBJ = $(SRC:%.c=%.o)
# binary file name
OUTPUT = $(TARGET)_$(HW_VER)_$(SW_VER)


# generate elf and bin
all: $(TARGET).elf $(OUTPUT).bin $(TARGET).lst map size

# generate elf file from objs
$(TARGET).elf: $(OBJ)
	@ $(ECHO) ---------------------    Linking   ---------------------
	$(CC) $(CC_FLAGS) $(OBJ) --output $@ $(LD_FLAGS) 

# generate bin file and generate checksum for it 
$(OUTPUT).bin: $(TARGET).elf
	@ $(ECHO) ---------------------    Copying   ---------------------
	$(OBC) $(OBC_FLAGS) $< $@
	$(CHKSUM) $(OUTPUT).bin

# geneate listing
$(TARGET).lst: $(TARGET).elf
	@ $(ECHO) ---------------------    Listing   ---------------------
	$(OBD) -S $< > $@

# compile all sources
%.o : %.c
	@ $(ECHO) --------------------- Compiling $< ---------------------
	$(CC) -c $(CC_FLAGS) $< -o $@

# show size information
size: $(TARGET).elf
	@ $(ECHO) --------------------- Section size ---------------------
	$(SIZE) $(TARGET).elf

# generate symbol map
map: $(TARGET).elf
	@ $(ECHO) --------------------- Symbol map ---------------------
	$(NM) -n -o $(TARGET).elf > $(TARGET).map


# cleanser xD
clean:
	$(RM) -f $(OBJ) $(OUTPUT).bin $(TARGET).lst $(TARGET).elf $(TARGET).map

