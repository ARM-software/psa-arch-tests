# Toolchain
PREFIX:=arm-none-eabi-
CC:=$(PREFIX)gcc
CXX:=$(PREFIX)g++
OBJCOPY:=$(PREFIX)objcopy
OBJDUMP:=$(PREFIX)objdump
AS:=$(PREFIX)as

# Root folder
ROOT_DIR:=.

PALTARDIR     :=../../../platform/board/${TARGET}/include/
FRAMEINCDIR   :=../../../platform/include/
DPMINCDIR     :=../../../platform/peripherals/dpm/fvp_basic/include/
NVRAMINCDIR   :=../../../platform/peripherals/nvram/nv_sim/include/
TIMERINCDIR   :=../../../platform/peripherals/timer/arm/include/
UARTINCDIR    :=../../../platform/peripherals/uart/arm/include/
WDTIMERINCDIR :=../../../platform/peripherals/watchdog/arm/include/
CMSISINCDIR   :=../../../platform/CMSIS_5/CMSIS/Core/Include/
DEVINCDIR     :=../../../platform/CMSIS_5/Device/ARM/ARMv8MML/Include/
CFLAGS        :=-march=armv8-m.main -mcmse -mthumb -Wall -Werror -O0 -fdata-sections -ffunction-sections \
                -I${PALTARDIR}    \
                -I${FRAMEINCDIR}  \
                -I${DPMINCDIR}    \
                -I${NVRAMINCDIR}  \
                -I${TIMERINCDIR}  \
                -I${UARTINCDIR}   \
                -I${WDTIMERINCDIR}\
                -I${CMSISINCDIR}  \
                -I${DEVINCDIR}

.PHONY: all

all: build

build: $(LIBNAME)

tbsa_pal_src  +=$(wildcard ../../../platform/board/${TARGET}/src/*.c)
tbsa_pal_src  +=$(wildcard ../../../platform/peripherals/dpm/fvp_basic/src/*.c)
tbsa_pal_src  +=$(wildcard ../../../platform/peripherals/nvram/nv_sim/src/*.c)
tbsa_pal_src  +=$(wildcard ../../../platform/peripherals/timer/arm/src/*.c)
tbsa_pal_src  +=$(wildcard ../../../platform/peripherals/uart/arm/src/*.c)
tbsa_pal_src  +=$(wildcard ../../../platform/peripherals/watchdog/arm/src/*.c)
tbsa_pal_src_s:=$(wildcard ../../../platform/board/${TARGET}/src/*.s)

tbsa_pal_obj  :=$(tbsa_pal_src:.c=.o)
tbsa_pal_obj  +=$(tbsa_pal_src_s:.s=.o)

$(LIBNAME): $(tbsa_pal_obj)
	$(AR) -rc $@ $^
	mv $@ ../../../$(ODIR)/.
	find $(ROOT_DIR) -iname '*.o' -delete
