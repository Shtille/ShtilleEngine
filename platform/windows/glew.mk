# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

include ../shtille-engine-sources.mk

SOURCES = $(SHT_PATH)/thirdparty/glew/src/glew.c
INCLUDE += -I$(SHT_PATH)/thirdparty/glew/include

CC = gcc
AR = ar

CFLAGS = -g -Wall -O3
CFLAGS += $(INCLUDE)
CFLAGS += -DGLEW_STATIC -DGLEW_NO_GLU

LDFLAGS = -s

OBJECTS = $(SOURCES:.c=.o)

TARGET = glew
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIBRARIES = -lstdc++ -lopengl32

all: $(SOURCES) glew
	echo All is done!

glew: create_dir $(STATIC_LIB)

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