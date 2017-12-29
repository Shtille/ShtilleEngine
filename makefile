
# 'saim' library and 'shtille earth' project has been temporary removed from compilation

ifeq ($(OS),Windows_NT)
    #CCFLAGS += -D WIN32
    MAKE := mingw32-make.exe
    THIRDPARTY_LIBRARIES = zlib png jpeg curl freetype glew bullet script
    PLATFORM_SUFFIX = mingw32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        #CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            #CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            #CCFLAGS += -D IA32
        endif
    endif
else
	MAKE := make
	THIRDPARTY_LIBRARIES = zlib png jpeg freetype bullet script
    INSTALL_PATH := lib
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        #CCFLAGS += -D LINUX
        PLATFORM_SUFFIX = unix
    endif
    ifeq ($(UNAME_S),Darwin)
        #CCFLAGS += -D OSX
        PLATFORM_SUFFIX = macosx
        # OSX has its own CURL with command line tools
        CURL_LIB := curl
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        #CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        #CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        #CCFLAGS += -D ARM
    endif
endif

### Thirdparty makefiles ###
ZLIB_MAKEFILE = sht/thirdparty/zlib/$(PLATFORM_SUFFIX).mk
PNG_MAKEFILE = sht/thirdparty/libpng/$(PLATFORM_SUFFIX).mk
JPEG_MAKEFILE = sht/thirdparty/libjpeg/$(PLATFORM_SUFFIX).mk
CURL_MAKEFILE = sht/thirdparty/libcurl/$(PLATFORM_SUFFIX).mk
FT_MAKEFILE = sht/thirdparty/freetype/$(PLATFORM_SUFFIX).mk
GLEW_MAKEFILE = sht/thirdparty/glew/$(PLATFORM_SUFFIX).mk
BULLET_MAKEFILE = sht/thirdparty/bullet/$(PLATFORM_SUFFIX).mk
SAIM_MAKEFILE = sht/thirdparty/libsaim/$(PLATFORM_SUFFIX).mk
SCRIPT_MAKEFILE = sht/thirdparty/script/$(PLATFORM_SUFFIX).mk

### Engine Makefile ###
ENGINE_MAKEFILE = sht/$(PLATFORM_SUFFIX).mk


export INSTALL_PATH

	#$(MAKE) -f apps/AtmosphericScattering/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/TestProject/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/PhysicsTest/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/BallsGame/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/ProjectX/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/MeshConverter/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/Billiard/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/IBLBaker/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/PBR/$(PLATFORM_SUFFIX).mk
	#$(MAKE) -f apps/ShtilleEarth/$(PLATFORM_SUFFIX).mk
all:
	$(MAKE) -f apps/Billiard/$(PLATFORM_SUFFIX).mk

engine:
	$(MAKE) -f $(ENGINE_MAKEFILE) IS_STATIC=YES

thirdparty: $(THIRDPARTY_LIBRARIES)

zlib:
	$(MAKE) -f $(ZLIB_MAKEFILE) IS_STATIC=YES

png:
	$(MAKE) -f $(PNG_MAKEFILE) IS_STATIC=YES

jpeg:
	$(MAKE) -f $(JPEG_MAKEFILE) IS_STATIC=YES

curl:
	$(MAKE) -f $(CURL_MAKEFILE) IS_STATIC=YES

freetype:
	$(MAKE) -f $(FT_MAKEFILE) IS_STATIC=YES

glew:
	$(MAKE) -f $(GLEW_MAKEFILE) IS_STATIC=YES

bullet:
	$(MAKE) -f $(BULLET_MAKEFILE) IS_STATIC=YES

saim:
	$(MAKE) -f $(SAIM_MAKEFILE) IS_STATIC=YES

script:
	$(MAKE) -f $(SCRIPT_MAKEFILE) IS_STATIC=YES