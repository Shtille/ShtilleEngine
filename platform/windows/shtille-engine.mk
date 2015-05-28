# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

include ../shtille-engine-sources.mk

DEFINES = -DGLEW_STATIC -DFREEIMAGE_LIB

CC = g++
AR = ar
CFLAGS = -g -Wall -std=c++11
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)
LDFLAGS = -g
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = ShtilleEngine
TARGET_PATH = ..\..\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so
LIBRARY_PATH = -L$(SHT_BIN)

LIBRARIES = -lstdc++ -lglew -lopengl32 -lgdi32 -lfreeimage

all: $(SOURCES) ShtilleEngine
	echo All is done!

clean:
	del ..\..\sht\*.o /s
	echo Clean is done!

ShtilleEngine: $(STATIC_LIB)
    
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)
	copy /Y $(STATIC_LIB) $(TARGET_PATH)\$(STATIC_LIB)
	del /Q $(STATIC_LIB)
	
$(SHARED_LIB): $(OBJECTS)
	$(CC) -s -shared $(LDFLAGS) -o $@ $^ $(LIBRARIES) $(LIBRARY_PATH)
	copy /Y $(SHARED_LIB) $(TARGET_PATH)\$(SHARED_LIB)
	del /Q $(SHARED_LIB)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@