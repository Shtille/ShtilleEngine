# Shtille Engine FreeType sources

SHT_PATH ?= ../../sht
SHT_BIN ?= ../../bin
FT_PATH = ../../sht/thirdparty/freetype/src

SOURCES = \
	$(FT_PATH)/autofit/autofit.c \
	$(FT_PATH)/bdf/bdf.c \
	$(FT_PATH)/cff/cff.c \
	$(FT_PATH)/base/ftbase.c \
	$(FT_PATH)/base/ftbitmap.c \
	$(FT_PATH)/cache/ftcache.c \
	$(FT_PATH)/base/ftfstype.c \
	$(FT_PATH)/base/ftgasp.c \
	$(FT_PATH)/base/ftglyph.c \
	$(FT_PATH)/gzip/ftgzip.c \
	$(FT_PATH)/base/ftinit.c \
	$(FT_PATH)/lzw/ftlzw.c \
	$(FT_PATH)/base/ftstroke.c \
	$(FT_PATH)/base/ftsystem.c \
	$(FT_PATH)/smooth/smooth.c \
	$(FT_PATH)/base/ftbbox.c \
	$(FT_PATH)/base/ftfntfmt.c \
	$(FT_PATH)/base/ftmm.c \
	$(FT_PATH)/base/ftpfr.c \
	$(FT_PATH)/base/ftsynth.c \
	$(FT_PATH)/base/fttype1.c \
	$(FT_PATH)/base/ftwinfnt.c \
	$(FT_PATH)/base/ftlcdfil.c \
	$(FT_PATH)/base/ftgxval.c \
	$(FT_PATH)/base/ftotval.c \
	$(FT_PATH)/base/ftpatent.c \
        $(FT_PATH)/base/ftxf86.c \
	$(FT_PATH)/pcf/pcf.c \
	$(FT_PATH)/pfr/pfr.c \
	$(FT_PATH)/psaux/psaux.c \
	$(FT_PATH)/pshinter/pshinter.c \
	$(FT_PATH)/psnames/psmodule.c \
	$(FT_PATH)/raster/raster.c \
	$(FT_PATH)/sfnt/sfnt.c \
	$(FT_PATH)/truetype/truetype.c \
	$(FT_PATH)/type1/type1.c \
	$(FT_PATH)/cid/type1cid.c \
	$(FT_PATH)/type42/type42.c \
	$(FT_PATH)/winfonts/winfnt.c \
	$(FT_PATH)/winfonts/ftdebug.c
	
INCLUDE = -I$(FT_PATH)/../include
	
DEFINES = -DFT2_BUILD_LIBRARY