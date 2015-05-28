# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

include ../freeimage-sources.mk

CC = gcc
CXX = g++
AR = ar

CFLAGS ?= -O3 -fexceptions -DNDEBUG -DDISABLE_PERF_MEASUREMENT -DWINVER=0x0500 $(LIB_TYPE_FLAGS) -DOPJ_STATIC
CFLAGS += $(INCLUDE)
#CFLAGS += $(DEFINES)

CXXFLAGS ?= -O3 -fexceptions -Wno-ctor-dtor-privacy -DNDEBUG -DWINVER=0x0500 $(LIB_TYPE_FLAGS) -DOPJ_STATIC -DLIBRAW_NODLL -include stdexcept

CXXFLAGS += $(INCLUDE)
#CXXFLAGS += $(DEFINES)

LDFLAGS ?= 
OBJECTS = $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.cpp=.o)
TARGET = FreeImage
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIBRARIES = -lws2_32

all: $(SOURCES) FreeImage
	echo All is done!

FreeImage: create_dir $(STATIC_LIB)

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