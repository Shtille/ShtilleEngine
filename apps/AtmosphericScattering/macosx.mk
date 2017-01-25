# Makefile for Unix

TARGET = AtmosphericScattering
TARGET_FILE = $(TARGET).app
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)/bin
APP_PATH = $(ROOT_PATH)/apps/$(TARGET)

CC = clang++
AR = ar rcs

CP = cp
RM = rm -f

INCLUDE = -I$(ROOT_PATH)/sht
DEFINES =

CFLAGS = -g -Wall -O3 -std=c++11
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS =

SRC_DIRS = $(APP_PATH)
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

OBJECTS = $(SRC_FILES:.cpp=.o)

LIBRARIES = -lShtilleEngine -framework Cocoa -framework OpenGL -framework Foundation -lstdc++ -lfreetype -ljpeg -lpng -lz

LIBRARY_PATH = -L$(INSTALL_PATH)

all: $(SRC_FILES) create_dir clean $(TARGET_FILE) install
	@echo All is done!

create_dir:
	@test -d $(TARGET_PATH) || mkdir $(TARGET_PATH)

clean:
	@find $(APP_PATH) -name "*.o" -type f -delete

install:
	@echo installing to $(TARGET_PATH)
	@$(RM) $(TARGET_PATH)/$(TARGET_FILE)
	@$(CP) $(TARGET_FILE) $(TARGET_PATH)/$(TARGET_FILE)
	@$(RM) $(TARGET_FILE)

$(TARGET_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET_FILE) $^ $(LIBRARIES) $(LIBRARY_PATH)

%.o : %.cpp
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@