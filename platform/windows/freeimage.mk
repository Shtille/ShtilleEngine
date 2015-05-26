# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

include ../freeimage-sources.mk

CC = g++
AR = ar
CFLAGS = -g -Wall -std=c++11
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)
LDFLAGS = -g
OBJECTS = $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.cpp=.o)
TARGET = FreeImage
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIBRARIES = -lstdc++

all: $(SOURCES) ShtilleEngine
	echo All is done!

ShtilleEngine: $(STATIC_LIB) $(SHARED_LIB)
    
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)
	copy /Y $(STATIC_LIB) $(TARGET_PATH)\$(STATIC_LIB)
	del /Q $(STATIC_LIB)
	
$(SHARED_LIB): $(OBJECTS)
	$(CC) -s -shared $(LDFLAGS) -o $@ $^ $(LIBRARIES)
	copy /Y $(SHARED_LIB) $(TARGET_PATH)\$(SHARED_LIB)
	del /Q $(SHARED_LIB)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@