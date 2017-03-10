# Makefile for Mac OS X

TARGET = script
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)/bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).dylib

LIB_PATH = $(ROOT_PATH)/sht/thirdparty/script

include $(LIB_PATH)/sources.mk

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

INCLUDE = -I$(LIB_PATH)/src
DEFINES = -DPARSER_WIDE_STRING

CFLAGS = -g -Wall -O3 -std=c++14
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -shared -fPIC

SRC_FILES_C = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
SRC_FILES_CPP = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
SRC_FILES = $(SRC_FILES_C) $(SRC_FILES_CPP)

OBJECTS_C = $(SRC_FILES_C:.c=.o)
OBJECTS_CPP = $(SRC_FILES:.cpp=.o)
OBJECTS = $(OBJECTS_C) $(OBJECTS_CPP)

LIBRARIES =

ifeq ($(INSTALL_PATH),)
INSTALL_PATH = $(TARGET_PATH)
endif

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(TARGET_TYPE) install

create_dir:
	@test -d $(INSTALL_PATH) || mkdir $(INSTALL_PATH)

clean:
	@find $(LIB_PATH) -name "*.o" -type f -delete

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
	@$(CC) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES)

%.o : %.c
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@

%.o : %.cpp
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@