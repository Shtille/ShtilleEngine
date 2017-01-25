# Makefile for Unix

TARGET = ShtilleEngine
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)/bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).dylib

include $(ROOT_PATH)/sht/sources.mk

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

CC = clang++
AR = ar rcs

CP = cp
RM = rm -f

DEFINES = -DGLEW_STATIC

CFLAGS = -g -Wall -O3
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)
CFLAGS_CPP = $(CFLAGS) -std=c++11

LDFLAGS = -shared -fPIC

SRC_FILES_M = $(ROOT_PATH)/sht/platform/src/macosx/fullscreen_window.m
SRC_FILES_MM = $(ROOT_PATH)/sht/platform/src/macosx/window_controller.mm
SRC_FILES_CPP = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
SRC_FILES = $(SRC_FILES_M) $(SRC_FILES_MM) $(SRC_FILES_CPP)

OBJECTS_M = $(SRC_FILES_M:.m=.o)
OBJECTS_MM = $(SRC_FILES_MM:.mm=.o)
OBJECTS_CPP = $(SRC_FILES_CPP:.cpp=.o)
OBJECTS = $(OBJECTS_M) $(OBJECTS_MM) $(OBJECTS_CPP)

LIBRARIES = -framework Cocoa -framework OpenGL -framework Foundation -lstdc++ -lfreetype -ljpeg -lpng -lz

ifeq ($(INSTALL_PATH),)
INSTALL_PATH = $(TARGET_PATH)
endif

LIBRARY_PATH = -L$(INSTALL_PATH)

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(TARGET_TYPE) install

create_dir:
	@test -d $(INSTALL_PATH) || mkdir $(INSTALL_PATH)

clean:
	@find $(ROOT_PATH)/sht -name "*.o" -type f -delete

install:
	@echo installing to $(INSTALL_PATH)
	@$(RM) $(INSTALL_PATH)/$(TARGET_FILE)
	@$(CP) $(TARGET_FILE) $(INSTALL_PATH)/$(TARGET_FILE)
	@$(RM) $(TARGET_FILE)
    
static: $(OBJECTS)
	@echo making static library
	@$(AR) $(STATIC_LIB) $(OBJECTS)
	
dynamic: $(OBJECTS)
	@echo making shared library
	@$(CC) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES) $(LIBRARY_PATH)

%.o : %.cpp
	@echo compiling file $<
	@$(CC) $(CFLAGS_CPP) -c $< -o $@

%.o : %.m
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@

%.o : %.mm
	@echo compiling file $<
	@$(CC) $(CFLAGS_CPP) -c $< -o $@