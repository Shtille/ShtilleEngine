# Shtille Engine sources makefile

SRC_DIRS = \
	$(ROOT_PATH)/sht/application \
	$(ROOT_PATH)/sht/application/opengl \
	$(ROOT_PATH)/sht/common \
	$(ROOT_PATH)/sht/containers \
	$(ROOT_PATH)/sht/geo/src \
	$(ROOT_PATH)/sht/graphics/src \
	$(ROOT_PATH)/sht/graphics/src/image \
	$(ROOT_PATH)/sht/graphics/src/model \
	$(ROOT_PATH)/sht/graphics/src/renderer \
	$(ROOT_PATH)/sht/graphics/src/renderer/opengl \
	$(ROOT_PATH)/sht/math \
	$(ROOT_PATH)/sht/platform/src \
	$(ROOT_PATH)/sht/system/src \
	$(ROOT_PATH)/sht/system/src/filesystem \
	$(ROOT_PATH)/sht/system/src/stream \
	$(ROOT_PATH)/sht/system/src/string \
	$(ROOT_PATH)/sht/system/src/tasks \
	$(ROOT_PATH)/sht/system/src/time \
	$(ROOT_PATH)/sht/utility/src \
	$(ROOT_PATH)/sht/utility/src/ui \
	#

INCLUDE = \
    -I$(ROOT_PATH)/sht \
    -I$(ROOT_PATH)/sht/thirdparty/libjpeg/include \
    -I$(ROOT_PATH)/sht/thirdparty/libjpeg/src \
	-I$(ROOT_PATH)/sht/thirdparty/libpng/include \
    -I$(ROOT_PATH)/sht/thirdparty/libpng/src \
    -I$(ROOT_PATH)/sht/thirdparty/freetype/include \
    -I$(ROOT_PATH)/sht/thirdparty/libcurl/include