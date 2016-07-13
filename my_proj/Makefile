
# Put your stlink folder here so make burn will work.
STLINK=/home/andrew/stlink

# Put your source files here (or *.c, etc)
SRCS  = main.c system_stm32f4xx.c misc.c my_usart.c cmdline.c util.c timer.c
SRCS += heartbeat.c my_gpio.c sched.c

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=main

# Put your STM32F4 library code directory here
STM_COMMON=../STM32F4-Discovery_FW_V1.1.0

# The compiler
CC=arm-none-eabi-gcc
# Objcopy for generating the different binaries
OBJCOPY=arm-none-eabi-objcopy


# Meaning of all of these flags:
# -I: tells gcc to look in the directory immediately following the "I" for
# include files
# -L: same thing as above but for linker files
# -g: compile with debug information
# -O2: specifies that gcc should optimize the code (O0-O3)
# -Wall: gives verbose compiler warnings
# -T: specifies the following file as a linker script
# -m: used to specify hardware/hardware configurations
# 	- little-endian: specifies compiling code for little-endian config
# 	- thumb: generate code for 16-bit Thumb inst set
# 	- cpu=cortex-m4: processor type
# 	- thumb-interwork: generate code for both Thumb and ARM inst sets
# 	- float-abi=hard: generate code with floating point inst
# 	- fpu=fpv4-sp-d16: another floating point specification, too lazy to find
CFLAGS  = -g -O2 -Wall -Tstm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -I.

# Include files from STM libraries
CFLAGS += -I$(STM_COMMON)/Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include 
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc

# add startup file to build
SRCS += $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s 
OBJS = $(SRCS:.c=.o)

# Ensures that 'make' runs regardless is there is already a file named whatever
# proj is defined as 
.PHONY: proj

# "all" specifies the target when 'make' is run without arguments
all: proj

proj: $(PROJ_NAME).elf

# $^: acts as a placeholder for all of the prerequisites used in the archive
# $@: acts as the name of the target in the archive
# -o: do not remake the file even if it is older than its prereqs

# -O: specifies the output target (here elf is src, hex or bin is dest)
$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

# if 'make clean' is entered, all of these files are removed before
# it attempts to compile the program.
clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32F4
burn: proj
	$(STLINK)/st-flash write $(PROJ_NAME).bin 0x8000000
