# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Jing Haihan

ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO is not set)
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

TARGET := feth-overlay
BUILD := build
SOURCES := \
	source/core \
	source/app \
	source/ui \
	libs/Atmosphere-libs/libstratosphere/source/dmnt
INCLUDES := \
	include \
	libs/Atmosphere-libs/libstratosphere/source/dmnt \
	libs/Atmosphere-libs/libstratosphere/source

APP_TITLE := FETH Overlay
APP_AUTHOR := Jing Haihan
APP_VERSION := $(shell tr -d '[:space:]' < $(TOPDIR)/VERSION)
NO_ICON := 1

ifeq ($(strip $(NO_NACP)),)
export NROFLAGS += --nacp=$(TOPDIR)/$(TARGET).nacp
endif

ARCH := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
CFLAGS := -g -Wall -O2 -ffunction-sections $(ARCH) $(DEFINES)
CFLAGS += $(INCLUDE) -D__SWITCH__
CXXFLAGS := $(CFLAGS) -std=gnu++20 \
	-isystem$(TOPDIR)/libs/libtesla/include \
	-DFETH_OVERLAY_VERSION=\"$(APP_VERSION)\"
ASFLAGS := -g $(ARCH)
LDFLAGS := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) \
	-Wl,-Map,$(notdir $*.map)
LIBS := -lnx
LIBDIRS := $(PORTLIBS) $(LIBNX)

.PHONY: all clean $(BUILD)

ifneq ($(BUILD),$(notdir $(CURDIR)))

all: $(BUILD)

export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export LD := $(CXX)
export OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
	-I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).nro $(TARGET).ovl \
		$(TARGET).nacp $(TARGET).map

else

DEPENDS := $(OFILES:.o=.d)

all: $(OUTPUT).ovl

$(OUTPUT).ovl: $(OUTPUT).elf $(OUTPUT).nacp
	@elf2nro $< $@ $(NROFLAGS)
	@echo "built ... $(notdir $@)"

$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif
