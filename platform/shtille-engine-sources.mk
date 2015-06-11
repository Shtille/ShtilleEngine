# Shtille Engine sources makefile

SHT_PATH ?= ../../sht
SHT_BIN ?= ../../bin

SOURCES ?= \
	$(SHT_PATH)/application/application.cpp \
	$(SHT_PATH)/application/opengl/opengl_application.cpp \
	$(SHT_PATH)/graphics/src/image/image.cpp \
      	$(SHT_PATH)/graphics/src/image/image_jpeg.cpp \
	$(SHT_PATH)/graphics/src/image/image_png.cpp \
	$(SHT_PATH)/graphics/src/renderer/opengl/opengl_renderer.cpp \
	$(SHT_PATH)/graphics/src/renderer/opengl/opengl_texture.cpp \
	$(SHT_PATH)/graphics/src/renderer/renderer.cpp \
	$(SHT_PATH)/graphics/src/renderer/texture.cpp \
	$(SHT_PATH)/graphics/src/renderer/vertex_format.cpp \
	$(SHT_PATH)/math/frustum.cpp \
	$(SHT_PATH)/math/matrix.cpp \
	$(SHT_PATH)/math/quaternion.cpp \
	$(SHT_PATH)/math/sht_math.cpp \
	$(SHT_PATH)/math/vector.cpp \
	$(SHT_PATH)/system/src/memory_leaks.cpp \
	$(SHT_PATH)/system/src/stream/file_stream.cpp \
	$(SHT_PATH)/system/src/stream/log_stream.cpp \
	$(SHT_PATH)/system/src/stream/memory_stream.cpp \
	$(SHT_PATH)/system/src/stream/stream.cpp \
	$(SHT_PATH)/system/src/string/filename.cpp \
	$(SHT_PATH)/system/src/time.cpp

INCLUDE = \
        -I$(SHT_PATH) \
        -I$(SHT_PATH)/thirdparty/libjpeg/include \
        -I$(SHT_PATH)/thirdparty/libjpeg/src \
	-I$(SHT_PATH)/thirdparty/libpng/include \
        -I$(SHT_PATH)/thirdparty/libpng/src