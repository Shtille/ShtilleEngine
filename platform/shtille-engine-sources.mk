# Shtille Engine sources makefile

SHT_PATH ?= ../../sht
SHT_BIN ?= ../../bin

SOURCES ?= \
	$(SHT_PATH)/application/application.cpp \
	$(SHT_PATH)/application/opengl/opengl_application.cpp \
	$(SHT_PATH)/graphics/src/model/cube_model.cpp \
	$(SHT_PATH)/graphics/src/model/tetrahedron_model.cpp \
    $(SHT_PATH)/graphics/src/model/sphere_model.cpp \
    $(SHT_PATH)/graphics/src/model/model.cpp \
	$(SHT_PATH)/graphics/src/image/image.cpp \
    $(SHT_PATH)/graphics/src/image/image_bmp.cpp \
    $(SHT_PATH)/graphics/src/image/image_jpeg.cpp \
	$(SHT_PATH)/graphics/src/image/image_png.cpp \
    $(SHT_PATH)/graphics/src/image/image_tga.cpp \
    $(SHT_PATH)/graphics/src/image/image_tif.cpp \
    $(SHT_PATH)/graphics/src/image/image_hdr.cpp \
	$(SHT_PATH)/graphics/src/renderer/opengl/opengl_context.cpp \
	$(SHT_PATH)/graphics/src/renderer/opengl/opengl_renderer.cpp \
	$(SHT_PATH)/graphics/src/renderer/opengl/opengl_texture.cpp \
	$(SHT_PATH)/graphics/src/renderer/context.cpp \
	$(SHT_PATH)/graphics/src/renderer/renderer.cpp \
	$(SHT_PATH)/graphics/src/renderer/texture.cpp \
    $(SHT_PATH)/graphics/src/renderer/shader.cpp \
	$(SHT_PATH)/graphics/src/renderer/index_buffer.cpp \
	$(SHT_PATH)/graphics/src/renderer/vertex_buffer.cpp \
	$(SHT_PATH)/graphics/src/renderer/vertex_format.cpp \
	$(SHT_PATH)/graphics/src/renderer/video_memory_buffer.cpp \
	$(SHT_PATH)/graphics/src/renderer/font.cpp \
	$(SHT_PATH)/graphics/src/renderer/text.cpp \
	$(SHT_PATH)/utility/src/ui/widget.cpp \
	$(SHT_PATH)/utility/src/ui/drawable_widget.cpp \
	$(SHT_PATH)/utility/src/ui/rect.cpp \
	$(SHT_PATH)/utility/src/ui/board.cpp \
	$(SHT_PATH)/utility/src/ui/label.cpp \
    $(SHT_PATH)/utility/src/ui/progress_bar.cpp \
	$(SHT_PATH)/utility/src/console.cpp \
	$(SHT_PATH)/utility/src/crypt.cpp \
	$(SHT_PATH)/utility/src/camera.cpp \
	$(SHT_PATH)/utility/src/curl_wrapper.cpp \
    $(SHT_PATH)/platform/src/main_wrapper.cpp \
    $(SHT_PATH)/platform/src/windows/window_controller.cpp \
	$(SHT_PATH)/math/frustum.cpp \
	$(SHT_PATH)/math/matrix.cpp \
	$(SHT_PATH)/math/quaternion.cpp \
	$(SHT_PATH)/math/sht_math.cpp \
	$(SHT_PATH)/math/vector.cpp \
	$(SHT_PATH)/system/src/stream/file_stream.cpp \
	$(SHT_PATH)/system/src/stream/log_stream.cpp \
	$(SHT_PATH)/system/src/stream/memory_stream.cpp \
	$(SHT_PATH)/system/src/stream/stream.cpp \
	$(SHT_PATH)/system/src/string/filename.cpp \
	$(SHT_PATH)/system/src/memory_leaks.cpp \
	$(SHT_PATH)/system/src/keys.cpp \
	$(SHT_PATH)/system/src/mouse.cpp \
	$(SHT_PATH)/system/src/time/update_timer.cpp \
    $(SHT_PATH)/system/src/time/scope_timer.cpp \
    $(SHT_PATH)/geo/src/planet_navigation.cpp

INCLUDE = \
    -I$(SHT_PATH) \
    -I$(SHT_PATH)/thirdparty/libjpeg/include \
    -I$(SHT_PATH)/thirdparty/libjpeg/src \
	-I$(SHT_PATH)/thirdparty/libpng/include \
    -I$(SHT_PATH)/thirdparty/libpng/src \
    -I$(SHT_PATH)/thirdparty/freetype/include \
    -I$(SHT_PATH)/thirdparty/libcurl/include