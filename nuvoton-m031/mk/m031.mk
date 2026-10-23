# Nuvoton M031 icin ortak Makefile (Keil yerine GCC + pyOCD)
# Her projenin Makefile'i bu dosyayi include eder.
#
#   make            -> derle (build/<proje>.elf/.hex/.bin)
#   make flash      -> karta yukle (Nu-Link2 uzerinden)
#   make erase      -> cipi tamamen sil
#   make reset      -> karti resetle
#   make monitor    -> seri port terminali (115200)
#   make probe      -> bagli Nu-Link'i listele
#   make clean

ROOT    := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
BSP     := $(ROOT)/M031BSP
DEV     := $(BSP)/Library/Device/Nuvoton/M031
PYOCD   := $(ROOT)/.venv/bin/pyocd

PROJECT ?= $(notdir $(CURDIR))

# Kartin uzerindeki cip (pyocd list --targets | grep m031 ile hepsini gor).
# Bilmiyorsan once 'make probe' / 'make info' calistir.
TARGET  ?= m032kg8ae

# Linker script: gcc_arm.ld = 16K flash / 2K RAM (tum M031'lerde calisir).
# Buyuk cipler icin: gcc_arm_128k.ld, gcc_arm_256k.ld
LDSCRIPT ?= $(DEV)/Source/GCC/gcc_arm_256k.ld

# Kaynaklar: projedeki tum .c dosyalari + BSP'nin tum surucusu
# (kullanilmayan kod --gc-sections ile atilir, surucu eklemekle ugrasma)
SRCS    := $(wildcard *.c) $(EXTRA_SRCS) \
           $(DEV)/Source/system_M031Series.c \
           $(wildcard $(BSP)/Library/StdDriver/src/*.c)
ASRCS   := $(DEV)/Source/GCC/startup_M031Series.S

CROSS   ?= arm-none-eabi-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
SIZE    := $(CROSS)size

BUILD   := build
OBJS    := $(addprefix $(BUILD)/,$(notdir $(SRCS:.c=.o) $(ASRCS:.S=.o)))
vpath %.c $(sort $(dir $(SRCS)))
vpath %.S $(sort $(dir $(ASRCS)))

ARCH    := -mcpu=cortex-m0 -mthumb
CFLAGS  += $(ARCH) -std=gnu11 -Os -g3 -Wall -fsigned-char \
           -ffunction-sections -fdata-sections \
           -I. -I$(BSP)/Library/CMSIS/Core/Include \
           -I$(DEV)/Include -I$(BSP)/Library/StdDriver/inc \
           -MMD -MP
ASFLAGS := $(ARCH) -x assembler-with-cpp
LDFLAGS := $(ARCH) -T$(LDSCRIPT) --specs=nano.specs --specs=nosys.specs \
           -Wl,--gc-sections,-Map=$(BUILD)/$(PROJECT).map,--no-warn-rwx-segments

.PHONY: all flash erase reset monitor probe info clean
all: $(BUILD)/$(PROJECT).hex $(BUILD)/$(PROJECT).bin
	@$(SIZE) $(BUILD)/$(PROJECT).elf

$(BUILD):
	@mkdir -p $@

$(BUILD)/%.o: %.c | $(BUILD)
	@echo "CC  $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S | $(BUILD)
	@echo "AS  $<"
	@$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/$(PROJECT).elf: $(OBJS)
	@echo "LD  $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	@$(OBJCOPY) -O ihex $< $@

$(BUILD)/%.bin: $(BUILD)/%.elf
	@$(OBJCOPY) -O binary $< $@

flash: all
	$(PYOCD) flash -t $(TARGET) $(BUILD)/$(PROJECT).hex

erase:
	$(PYOCD) erase -t $(TARGET) --chip

reset:
	$(PYOCD) reset -t $(TARGET)

probe:
	$(PYOCD) list

info:
	$(PYOCD) commander -t $(TARGET) -c "show target" -c "show cores"

# Nu-Link VCOM portunu bulup acar. Cikis: Ctrl+A, sonra K, sonra Y
monitor:
	@PORT=$$(ls /dev/cu.usbmodem* 2>/dev/null | head -1); \
	if [ -z "$$PORT" ]; then echo "Seri port yok. Kart bagli mi, Nu-Link VCOM switch'leri ON mu?"; exit 1; fi; \
	echo "$$PORT acılıyor (115200). Cikis: Ctrl+A, K, Y"; screen $$PORT 115200

clean:
	rm -rf $(BUILD)

-include $(OBJS:.o=.d)
