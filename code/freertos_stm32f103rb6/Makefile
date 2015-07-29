PREFIX = arm-none-eabi

CC      = arm-none-eabi-gcc
CPP     = arm-none-eabi-g++
LD      = arm-none-eabi-gcc
CP      = arm-none-eabi-objcopy
OD      = arm-none-eabi-objdump

TOOLCHAIN_DIR := $(shell dirname `which $(CC)`)/../$(PREFIX)

CFLAGS		+= -Os -g -Wall -I$(TOOLCHAIN_DIR)/include -Ilibopencm3 \
		   -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD -DSTM32F1 -D__TARGET_PROCESSOR=STM32F103RB6 -specs=nosys.specs

LDFLAGS		+= -Wl,--start-group -lc -lgcc -Wl,--end-group \
		   -nostartfiles -Wl,--gc-sections \
			-L./ -L$(TOOLCHAIN_DIR)/lib -L$(TOOLCHAIN_DIR)/lib/stm32/f1  \
		   -mthumb -march=armv7 -mfix-cortex-m3-ldrd -msoft-float -specs=nosys.specs 
		   
		   

		   
ALL_CFLAGS := -g -Wall -I./ -Iinc  -c $(CFLAGS) $(CC_FLAGS) -Os



C_FILES := $(wildcard src/*.c)
CPP_FILES := $(wildcard src/*.cpp)
ASM_FILES := $(wildcard src/*.s)

OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))
OBJ_FILES += $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
OBJ_FILES += $(addprefix obj/,$(notdir $(ASM_FILES:.s=.o)))


PROGRAM_NAME := test





all: $(PROGRAM_NAME)

clean:
	rm -rf *.lst obj/*.o obj/*.d *.elf *.bin obj

$(PROGRAM_NAME):$(PROGRAM_NAME).bin

$(PROGRAM_NAME).bin: $(PROGRAM_NAME).elf
	@ echo "[Copying]"
	$(CP) -Obinary  $(PROGRAM_NAME).elf $(PROGRAM_NAME).bin
	$(OD) -S $(PROGRAM_NAME).elf > $(PROGRAM_NAME).lst

	
obj/%.o: src/%.s
	@mkdir -p obj
	@echo "\n[Compiling]" $< "\n" 
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 
	
obj/%.o: src/%.c
	@mkdir -p obj
	@echo "[Compiling]" $< 
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 
	
obj/%.o: src/%.cpp
	@mkdir -p obj
	@echo "[Compiling]" $< 
	$(CPP) -c $(ALL_CFLAGS) $< -o $@ 
	
$(PROGRAM_NAME).elf: $(OBJ_FILES)
	@ echo "[Linking]"
	$(LD) -T stm32f103rb6.ld $(LDFLAGS) $^ -lopencm3_stm32f1 -o $(PROGRAM_NAME).elf

	

	
# main.o: main.cpp
# 	@ echo "[Compiling main code]"
# 	$(CC) -g -Wall -pedantic -I./ -c $(CFLAGS) main.cpp -Os

prog: $(PROGRAM_NAME)
	sudo st-flash write v1 $(PROGRAM_NAME).bin 0x08000000

	
CC_FLAGS += -MMD
-include $(OBJ_FILES:.o=.d)

