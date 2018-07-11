# Toolchain
PREFIX:=arm-none-eabi-
CC:=$(PREFIX)gcc
CXX:=$(PREFIX)g++
AS:=$(PREFIX)as
OBJCOPY:=$(PREFIX)objcopy
OBJDUMP:=$(PREFIX)objdump

# Test specific macros
OUT_DIR:=./out

# Root folder
ROOT_DIR:=.

.PHONY: all clean

VAL_LIB   :=../../out/tbsa_val.a
PAL_LIB   :=../../out/tbsa_pal.a
SYSLIBS   :=$(VAL_LIB) $(PAL_LIB) -lgcc -lc -lnosys
INCDIR    := ../../val/include/
CFLAGS    :=-march=armv8-m.main -mthumb -Wall -Werror -O0 -I$(INCDIR)
SBUILD    :=-mcmse

LDFLAGS   :=\
          -Xlinker --fatal-warnings \
          -Xlinker --gc-sections \
          -z max-page-size=0x400

SFLAGS    :=\
          $(CFLAGS) \
          $(SBUILD) \
          -nostartfiles \
          -T../../platform/board/${TARGET}/linker/test_s.linker \
          $(LDFLAGS)

NSFLAGS   :=\
          $(CFLAGS) \
          -nostartfiles \
          -T../../platform/board/${TARGET}/linker/test_ns.linker \
          $(LDFLAGS)

s_src     :=secure.c
ns_src    :=non_secure.c

DIR       :=$(dir $(wildcard $(ROOT_DIR)/*/))
DIR       :=$(filter-out ./, $(DIR))
all: build

build:
	$(foreach d, $(DIR), mkdir -p $(d)$(OUT_DIR); \
                  $(CC) $(SFLAGS) -Xlinker -Map=$(d)$(OUT_DIR)/$(s_src).map -o $(d)$(OUT_DIR)/$(s_src).elf $(d)$(s_src) $(SYSLIBS); \
                  $(OBJCOPY) $(d)$(OUT_DIR)/$(s_src).elf -O binary $(d)$(OUT_DIR)/$(s_src).bin; \
                  $(OBJDUMP) -d $(d)$(OUT_DIR)/$(s_src).elf > $(d)$(OUT_DIR)/$(s_src).asm; \
                  $(CC) $(NSFLAGS) -Xlinker -Map=$(d)$(OUT_DIR)/$(ns_src).map -o $(d)$(OUT_DIR)/$(ns_src).elf $(d)$(ns_src) $(SYSLIBS); \
                  $(OBJCOPY) $(d)$(OUT_DIR)/$(ns_src).elf -O binary $(d)$(OUT_DIR)/$(ns_src).bin; \
                  $(OBJDUMP) -d $(d)$(OUT_DIR)/$(ns_src).elf > $(d)$(OUT_DIR)/$(ns_src).asm;)

clean:
	$(foreach d, $(DIR), rm -rf $(d)$(OUT_DIR))
