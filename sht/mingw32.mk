# Makefile for Windows

TARGET = ShtilleEngine
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

include $(ROOT_PATH)/sht/sources.mk

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

CC = g++
AR = ar rcs

CP = @copy /Y
RM = @del /Q

DEFINES = -DGLEW_STATIC

CFLAGS = -g -Wall -O3 -std=c++11
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s -shared

PLATFORM_SPECIFIC_FILES = $(ROOT_PATH)/sht/platform/src/windows/window_controller.cpp
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
SRC_FILES += $(PLATFORM_SPECIFIC_FILES)

OBJECTS = $(SRC_FILES:.cpp=.o)

LIBRARIES = -lstdc++ -lgdi32 -lglew -lopengl32 -lfreetype -ljpeg -lpng -lz

ifeq ($(INSTALL_PATH),)
INSTALL_PATH = $(TARGET_PATH)
endif

LIBRARY_PATH = -L$(INSTALL_PATH)

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(TARGET_TYPE) install

create_dir:
	@if not exist $(INSTALL_PATH) mkdir $(INSTALL_PATH)

clean:
	@for /r %%R in ($(ROOT_PATH)\sht\*.o) do (if exist %%R del /Q %%R)

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
	@$(CC) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES) $(LIBRARY_PATH)

%.o : %.cpp
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@