# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

SHT_PATH = ../../sht
SHT_BIN = ../../bin

SRC_DIRS = ../../apps/AtmosphericScattering
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

INCLUDE = -I$(SHT_PATH)

CC = g++
CFLAGS = -g -Wall -std=c++11
CFLAGS += $(INCLUDE)
LDFLAGS = -s -mwindows
OBJECTS = $(SRC_FILES:.cpp=.o)
TARGET = AtmosphericScattering
TARGET_PATH = ..\..\bin
EXECUTABLE = $(TARGET).exe

LIBRARIES = \
	-lstdc++ -lgdi32 \
	-lShtilleEngine \
        -lglew -lopengl32 \
        -lfreetype -ljpeg -lpng -lz

all: $(SRC_FILES) $(EXECUTABLE)
	echo All is done!
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBRARIES) -L$(SHT_BIN)
	copy /Y $(EXECUTABLE) $(TARGET_PATH)\$(EXECUTABLE)
	del /Q $(EXECUTABLE)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@