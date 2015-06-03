# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

LIB_PATH = ../../sht/thirdparty/libpng/src

include ../libpng-sources.mk

INCLUDE = \
	-I$(LIB_PATH) \
	-I$(LIB_PATH)/../include \
	-I$(LIB_PATH)/../../zlib/include \
	-I$(LIB_PATH)/../../zlib/src

DEFINES = -DPNG_USER_WIDTH_MAX=16384 -DPNG_USER_HEIGHT_MAX=16384

CC = gcc
AR = ar

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s

OBJECTS = $(SOURCES:.c=.o)

TARGET = png
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIBRARIES = -lstdc++ -lz

all: $(SOURCES) png
	echo All is done!

png: create_dir $(STATIC_LIB)

create_dir:
	if not exist $(TARGET_PATH) mkdir $(TARGET_PATH)
    
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)
	copy /Y $(STATIC_LIB) $(TARGET_PATH)\$(STATIC_LIB)
	del /Q $(STATIC_LIB)
	
$(SHARED_LIB): $(OBJECTS)
	$(CC) -s -shared $(LDFLAGS) -o $@ $^ $(LIBRARIES)
	copy /Y $(SHARED_LIB) $(TARGET_PATH)\$(SHARED_LIB)
	del /Q $(SHARED_LIB)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@