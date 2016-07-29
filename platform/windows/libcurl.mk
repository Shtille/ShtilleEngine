# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

TARGET = curl
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

CC = gcc
AR = ar

LIB_PATH = ../../sht/thirdparty/libcurl/src

INCLUDE += -I$(LIB_PATH) -I$(LIB_PATH)/../include

DEFINES = -DBUILDING_LIBCURL -DCURL_STATICLIB

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s

SRC_DIRS = $(LIB_PATH)
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJECTS = $(SRC_FILES:.c=.o)

LIBRARIES = -lstdc++

all: $(SRC_FILES) curl
	echo All is done!

curl: create_dir $(STATIC_LIB)

create_dir:
	@if not exist $(TARGET_PATH) mkdir $(TARGET_PATH)
    
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