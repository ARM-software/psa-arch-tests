# Toolchain
PREFIX:=arm-none-eabi-
CC:=$(PREFIX)gcc
CXX:=$(PREFIX)g++
OBJCOPY:=$(PREFIX)objcopy
OBJDUMP:=$(PREFIX)objdump
AS:=$(PREFIX)as

# Test specific macros
OUT_DIR :=./out
VLIB    :=tbsa_val.a
PLIB    :=tbsa_pal.a

SUITE :=

# Root folder
ROOT_DIR:=.

VALINCDIR     :=val/include/
CFLAGS        :=-march=armv8-m.main -mcmse -mthumb -Wall -Werror -O0 -fdata-sections -ffunction-sections -I${VALINCDIR}
ASFLAGS       :=-march=armv8-m.main -mthumb
LDFLAGS       :=\
               -Xlinker --fatal-warnings \
               -Xlinker --gc-sections \
               -z max-page-size=0x400 \
               -Xlinker -Map=tbsa.map

SYSLIBS       :=$(OUT_DIR)/$(VLIB) $(OUT_DIR)/$(PLIB) -lgcc -lc -lnosys

.PHONY: all clean

all: check_target clean gen_database gen_linker setup_cmsis build

check_target:
	@if [ ! -d "./platform/board/$(TARGET)/" ]; then { echo "Target Not Found!!!"; exit 1; } fi

gen_database:
	@echo ""
	@echo "Reading target configuration file ..."
	mkdir -p platform/board/${TARGET}/include
	./tools/tgt_cfg_parser/targetConfigGen.pl ./platform/board/${TARGET}/tbsa_tgt.cfg platform/board/${TARGET}/include/pal_database
	gcc targetConfigGen.c -o targetConfigGen -I./${VALINCDIR}
	./targetConfigGen
	rm -f ./targetConfigGen
	rm -f ./targetConfigGen.c

gen_linker:
	@echo ""
	@echo "Generating target specific linker scripts ..."
	cd tools/gen_linker_scripts/ && python gen_linker_scripts.py ${TARGET}
	@echo ""

setup_cmsis:
	@if [ ! -d "./platform/CMSIS_5" ]; then\
		cd platform && git clone https://github.com/ARM-software/CMSIS_5.git && cd -;\
		cd platform/CMSIS_5 && git checkout 49ac527aa7406cecbba46d4d3bdbc7f60c6c6d42;\
	else	echo "CMSIS already cloned"; echo "";\
	fi

build: output pal_lib $(VLIB) tbsa.bin

tbsa_val_src  :=$(wildcard val/src/*.c)
tbsa_val_src_s:=$(wildcard val/src/asm/*.s)

tbsa_val_obj  :=$(tbsa_val_src:.c=.o)
tbsa_val_obj  +=$(tbsa_val_src_s:.s=.o)

output:
	mkdir -p $(OUT_DIR)

pal_lib:
	cd platform/board/${TARGET} && make ODIR=$(OUT_DIR) LIBNAME=$(PLIB)

$(VLIB): $(tbsa_val_obj)
	$(AR) -rc $(OUT_DIR)/$@ $^
	find $(ROOT_DIR) -iname '*.o' -delete

clean:
	rm -rf $(OUT_DIR)
	find $(ROOT_DIR) -iname '*.o' -delete
	cd test_pool && make clean

tbsa.bin:tbsa.elf
	$(OBJCOPY) $< -j .text -O binary $@
	mv tbsa.asm $(OUT_DIR)
	mv tbsa.bin $(OUT_DIR)
	mv tbsa.elf $(OUT_DIR)
	mv tbsa.hex $(OUT_DIR)
	mv tbsa.map $(OUT_DIR)
	cd test_pool && make

tbsa.elf:tbsa_app/tbsa_main.c
	$(CC) $(CFLAGS) -Tplatform/board/${TARGET}/linker/tbsa.linker $(LDFLAGS) -o $@ tbsa_app/tbsa_main.c $(SYSLIBS)
	$(OBJDUMP) -d $@ > tbsa.asm
	hexdump -v -e ' 1/4 "%08X" "\n"' tbsa.elf > tbsa.hex
