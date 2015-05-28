# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

include ../freeimage-sources.mk

CC = gcc
CXX = g++
AR = ar

CFLAGS ?= -O3 -fPIC -fexceptions -fvisibility=hidden
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

CXXFLAGS ?= -O3 -fPIC -fexceptions -fvisibility=hidden -Wno-ctor-dtor-privacy

CXXFLAGS += $(INCLUDE)
CXXFLAGS += $(DEFINES)

LDFLAGS ?= -g
OBJECTS = $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.cpp=.o)
TARGET = FreeImage
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIBRARIES = -lstdc++

all: $(SOURCES) FreeImage
	echo All is done!

FreeImage: create_dir $(STATIC_LIB)

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

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@