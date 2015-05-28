# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

SHT_PATH = ../../sht
SHT_BIN = ../../bin

SOURCES = ../../TestProject/Source.cpp

INCLUDE = -I$(SHT_PATH)

CC = g++
CFLAGS = -g -Wall -std=c++11
CFLAGS += $(INCLUDE)
LDFLAGS = -g
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = TestProject
TARGET_PATH = ..\..\bin
EXECUTABLE = $(TARGET).exe

LIBRARIES = \
	-lstdc++ \
	-lShtilleEngine -lglew -lopengl32 -lgdi32 -lfreeimage -lws2_32

all: $(SOURCES) $(EXECUTABLE)
	echo All is done!
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBRARIES) -L$(SHT_BIN)
	copy /Y $(EXECUTABLE) $(TARGET_PATH)\$(EXECUTABLE)
	del /Q $(EXECUTABLE)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@