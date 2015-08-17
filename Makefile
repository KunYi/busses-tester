#
# Copyright(c) Microsoft Open Technologies, Inc. All rights reserved.
#
# The MIT License(MIT)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
.SUFFIXES:
.SUFFIXES: .c .cpp .S .o .hex .elf .bin .a

MBED_ROOT=mbed

CPU= -mcpu=cortex-m3 -mthumb
CFLAGS= \
    $(CPU) \
    -c \
    -g \
    -fno-common \
    -fmessage-length=0 \
#    -Wall \
    -Wextra \
    -fno-exceptions \
    -ffunction-sections \
    -fdata-sections \
    -fomit-frame-pointer \
#    -MMD \  nmake can't include dynamically generated files
#    -MP \

ASFLAGS=$(CFLAGS)
LDFLAGS= \
    $(CPU) \
    -Wl,--gc-sections \
    --specs=nano.specs \
    -u _printf_float \
    -u _scanf_float \
    -Wl,--wrap,main \

LDSCRIPT=$(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld
LIBRARY_PATHS= -L$(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM 

# default optimization, overridable in sources.inc files
OPT= -Os

CDEFINES= \
    -DTARGET_LPC1768 \
    -DTARGET_M3 \
    -DTARGET_CORTEX_M \
    -DTARGET_NXP \
    -DTARGET_LPC176X \
    -DTARGET_MBED_LPC1768 \
    -DTOOLCHAIN_GCC_ARM \
    -DTOOLCHAIN_GCC \
    -D__CORTEX_M3 \
    -DARM_MATH_CM3 \
    -DMBED_BUILD_TIMESTAMP=1439405352.85 \
    -D__MBED__=1 \
    
CINCS= \
    -I$(MBED_ROOT) \
    -I$(MBED_ROOT)/TARGET_LPC1768 \
    -I$(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM \
    -I$(MBED_ROOT)/TARGET_LPC1768/TARGET_NXP \
    -I$(MBED_ROOT)/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X \
    -I$(MBED_ROOT)/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 \

STDLIBS= -lnosys -lmbed

SYS_OBJECTS= \
    $(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/board.o \
    $(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o \
    $(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/cmsis_nvic.o \
    $(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/retarget.o \
    $(MBED_ROOT)/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o \

PFX=arm-none-eabi
CC=$(PFX)-gcc
CPP=$(PFX)-g++
OBJCOPY=$(PFX)-objcopy
OBJDUMP=$(PFX)-objdump
SIZE=$(PFX)-size
NM=$(PFX)-nm

# include the project specific definitions
!include sources.mak

OBJ= $(CSRC:.c=.o) $(ASRC:.S=.o) $(CPPSRC:.cpp=.o) $(SYS_OBJECTS)

!if "$(TARGETTYPE)" == "program"
TARGET=$(TARGETNAME).bin
CFLAGS=$(CFLAGS) -Werror
CPPFLAGS=$(CPPFLAGS) -Werror
!else if "$(TARGETTYPE)" == "library"
TARGET=$(TARGETNAME).a
!endif

all: $(TARGET)

$(TARGETNAME).elf: $(OBJ) $(LIBS)
    @echo.
    @echo Linking $@
    $(CC) $(LDFLAGS) -Wl,-Map=$(TARGETNAME).map,--cref $(OPT) -T$(LDSCRIPT) $(LIBRARY_PATHS) -o $@ $(OBJ) $(LIBS) -lmbed
    @echo Size of executable:
    @$(SIZE) -B -d $(TARGETNAME).elf

$(TARGETNAME).a: $(OBJ)
    $(AR) r $@ $?

.elf.bin:
    @echo.
    @echo Creating BIN
    $(OBJCOPY) -O binary $< $@
    
.elf.hex:
    @echo.
    @echo Creating HEX
    $(OBJCOPY) -O ihex $< $@

.c.o:
    @echo.
    @echo Compiling $<
    $(CC) -c -std=cll $(CFLAGS) $(OPT) $(CDEFINES) $(CINCS) $<

.cpp.o:
    @echo.
    @echo Compiling $<
    $(CPP) -c -std=c++11 -fno-rtti $(CFLAGS) $(OPT) $(CDEFINES) $(CINCS) $<

.S.o:
    $(CC) -c -x assembler-with-cpp $(ASFLAGS) $(OPT) -o $@ $<

.PHONY: clean
clean:
    del /q *.o *.a *.lst *.elf *.bin *.hex *.d *.map $(CLEANFILES)
