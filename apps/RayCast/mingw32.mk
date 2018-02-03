# Makefile for Windows

TARGET = RayCast
TARGET_FILE = $(TARGET).exe
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)\bin
APP_PATH = $(ROOT_PATH)\apps\$(TARGET)

CC = g++
AR = ar rcs

CP = @copy /Y
RM = @del /Q

INCLUDE = -I$(ROOT_PATH)/sht
DEFINES =

CFLAGS = -g -Wall -O3 -std=c++11
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s -mwindows

SRC_DIRS = $(APP_PATH)
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

OBJECTS = $(SRC_FILES:.cpp=.o)

LIBRARIES = -lShtilleEngine -lstdc++ -lgdi32 -lglew -lopengl32 -lfreetype -ljpeg -lpng -lz

LIBRARY_PATH = -L$(INSTALL_PATH)

all: $(SRC_FILES) create_dir clean $(TARGET_FILE) install
	@echo All is done!

create_dir:
	@if not exist $(TARGET_PATH) mkdir $(TARGET_PATH)

clean:
	@for /r %%R in ($(APP_PATH)\*.o) do (if exist %%R del /Q %%R)

install:
	@echo installing to $(TARGET_PATH)
	@$(RM) $(TARGET_PATH)\$(TARGET_FILE)
	@$(CP) $(TARGET_FILE) $(TARGET_PATH)\$(TARGET_FILE)
	@$(RM) $(TARGET_FILE)

$(TARGET_FILE): $(OBJECTS)
	@echo linking $(TARGET)
	@$(CC) $(LDFLAGS) -o $(TARGET_FILE) $^ $(LIBRARIES) $(LIBRARY_PATH)

%.o : %.cpp
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@