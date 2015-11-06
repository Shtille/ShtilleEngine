# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

include ../freetype-sources.mk

CC = gcc
AR = ar

CFLAGS ?= -g -Wall -O3
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS ?= 
OBJECTS = $(SOURCES:.c=.o)
TARGET = freetype
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIBRARIES ?=

all: $(SOURCES) FreeType
	echo All is done!

FreeType: create_dir $(STATIC_LIB)

create_dir:
	if not exist $(TARGET_PATH) mkdir $(TARGET_PATH)
    
$(STATIC_LIB): $(OBJECTS)
	$(AR) rs $@ $(OBJECTS)
	copy /Y $(STATIC_LIB) $(TARGET_PATH)\$(STATIC_LIB)
	del /Q $(STATIC_LIB)
	
$(SHARED_LIB): $(OBJECTS)
	$(CC) -s -shared $(LDFLAGS) -o $@ $^ $(LIBRARIES)
	copy /Y $(SHARED_LIB) $(TARGET_PATH)\$(SHARED_LIB)
	del /Q $(SHARED_LIB)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@