# Makefile for Windows

TARGET = curl
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIB_PATH = $(ROOT_PATH)/sht/thirdparty/libcurl/src

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

CC = gcc
AR = ar rcs

CP = @copy /Y
RM = @del /Q

INCLUDE += -I$(LIB_PATH)/../include \
		   -I$(LIB_PATH)

DEFINES = -DBUILDING_LIBCURL -DCURL_STATICLIB

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s -shared

SRC_DIRS = $(LIB_PATH)
SRC_DIRS += $(LIB_PATH)/vauth
SRC_DIRS += $(LIB_PATH)/vtls
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJECTS = $(SRC_FILES:.c=.o)

LIBRARIES = -lWs2_32 -lWldap32

INSTALL_PATH = $(TARGET_PATH)

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(TARGET_TYPE) install

create_dir:
	@if not exist $(INSTALL_PATH) mkdir $(INSTALL_PATH)

clean:
	@for /r %%R in (*.o) do (if exist %%R del /Q %%R)

install:
	@echo installing to $(INSTALL_PATH)
	@$(RM) $(INSTALL_PATH)\$(TARGET_FILE)
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