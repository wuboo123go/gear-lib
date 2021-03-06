
#ARCH: linux/pi/android/ios/
ARCH		?= linux
CROSS_PREFIX	?=
OUTPUT		?= /usr/local
BUILD_DIR	:= $(shell pwd)/../../build/
ARCH_INC	:= $(BUILD_DIR)/$(ARCH).inc
COLOR_INC	:= $(BUILD_DIR)/color.inc

ifeq ($(ARCH_INC), $(wildcard $(ARCH_INC)))
include $(ARCH_INC)
endif

CC	= $(CROSS_PREFIX)gcc
CXX	= $(CROSS_PREFIX)g++
LD	= $(CROSS_PREFIX)ld
AR	= $(CROSS_PREFIX)ar
STRIP   = $(CROSS_PREFIX)strip

ifeq ($(COLOR_INC), $(wildcard $(COLOR_INC)))
include $(COLOR_INC)
else
CC_V	= $(CC)
CXX_V	= $(CXX)
LD_V	= $(LD)
AR_V	= $(AR)
CP_V	= $(CP)
RM_V	= $(RM)
endif

########
LIBNAME		= librbtree
TGT_LIB_H	= $(LIBNAME).h
TGT_LIB_A	= $(LIBNAME).a
TGT_LIB_SO	= $(LIBNAME).so
TGT_UNIT_TEST	= test_$(LIBNAME)

OBJS_LIB	= $(LIBNAME).o
OBJS_UNIT_TEST	= test_$(LIBNAME).o

ifeq ($(MODE), release)
CFLAGS	:= -O2 -Wall -Werror -fPIC
LTYPE   := release
else
CFLAGS	:= -g -Wall -Werror -fPIC
LTYPE   := debug
endif
ifeq ($(OUTPUT),/usr/local)
OUTLIBPATH :=/usr/local
else
OUTLIBPATH :=$(OUTPUT)/$(LTYPE)
endif
CFLAGS	+= $($(ARCH)_CFLAGS)
CFLAGS	+= -I$(OUTPUT)/include

SHARED	:= -shared

LDFLAGS	:= $($(ARCH)_LDFLAGS)
LDFLAGS	+= -pthread

.PHONY : all clean

TGT	:= $(TGT_LIB_A)
TGT	+= $(TGT_LIB_SO)
TGT	+= $(TGT_UNIT_TEST)

OBJS	:= $(OBJS_LIB) $(OBJS_UNIT_TEST)

all: $(TGT)

%.o:%.c
	$(CC_V) -c $(CFLAGS) $< -o $@

$(TGT_LIB_A): $(OBJS_LIB)
	$(AR_V) rcs $@ $^

$(TGT_LIB_SO): $(OBJS_LIB)
	$(LD_V) -o $@ $^ $(SHARED)

$(TGT_UNIT_TEST): $(OBJS_UNIT_TEST) $(ANDROID_MAIN_OBJ)
	$(CC_V) -o $@ $^ $(TGT_LIB_A) $(LDFLAGS)

clean:
	$(RM_V) -f $(OBJS)
	$(RM_V) -f $(TGT)

install:
	$(MAKEDIR_OUTPUT)
	if [ "$(MODE)" = "release" ];then $(STRIP) $(TGT); fi
	$(CP_V) -r $(TGT_LIB_H)  $(OUTPUT)/include
	$(CP_V) -r $(TGT_LIB_A)  $(OUTLIBPATH)/lib
	$(CP_V) -r $(TGT_LIB_SO) $(OUTLIBPATH)/lib

uninstall:
	cd $(OUTPUT)/include/ && rm -f $(TGT_LIB_H)
	$(RM_V) -f $(OUTLIBPATH)/lib/$(TGT_LIB_A)
	$(RM_V) -f $(OUTLIBPATH)/lib/$(TGT_LIB_SO)
