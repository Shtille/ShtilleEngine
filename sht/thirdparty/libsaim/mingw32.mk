# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

TARGET = saim
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

ifeq ($(INSTALL_PATH),)
INSTALL_PATH = $(TARGET_PATH)
endif

CC = gcc
AR = ar rcs

CP = @copy /Y
RM = @del /Q

LIB_PATH = $(ROOT_PATH)/sht/thirdparty/libsaim/src

INCLUDE += -I$(LIB_PATH) -I$(LIB_PATH)/../include
ifneq ($(CURL_PATH),)
INCLUDE += -I$(CURL_PATH)\include
endif
ifneq ($(JPEG_PATH),)
INCLUDE += -I$(JPEG_PATH)\include
endif
ifneq ($(PNG_PATH),)
INCLUDE += -I$(PNG_PATH)\include
endif

DEFINES = -DBUILDING_LIBSAIM
ifneq ($(IS_STATIC),NO)
DEFINES += -DSAIM_STATICLIB
endif
DEFINES += -DCURL_STATICLIB

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s -shared

SRC_DIRS = $(LIB_PATH)
SRC_DIRS += $(LIB_PATH)/rasterizer
SRC_DIRS += $(LIB_PATH)/util
SRC_DIRS += $(LIB_PATH)/../deps
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJECTS = $(SRC_FILES:.c=.o)

LIBRARIES = -L$(INSTALL_PATH)
ifneq ($(CURL_LIB),)
LIBRARIES += -l$(CURL_LIB)
endif
ifneq ($(JPEG_LIB),)
LIBRARIES += -l$(JPEG_LIB)
endif
ifneq ($(PNG_LIB),)
LIBRARIES += -l$(PNG_LIB)
endif
ifneq ($(Z_LIB),)
LIBRARIES += -l$(Z_LIB)
endif
LIBRARIES += -lWs2_32 -lWldap32

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(TARGET_TYPE) install

create_dir:
	@if not exist $(INSTALL_PATH) mkdir $(INSTALL_PATH)

clean:
	@for /r %%R in (*.o) do (if exist %%R del /Q %%R)

install:
	@echo installing to $(INSTALL_PATH)
	@if exist $(INSTALL_PATH)\$(TARGET_FILE) $(RM) $(INSTALL_PATH)\$(TARGET_FILE)
	@$(CP) $(TARGET_FILE) $(INSTALL_PATH)\$(TARGET_FILE)
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