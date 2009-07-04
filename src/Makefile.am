bin_PROGRAMS = sear-bin
bin_SCRIPTS = sear
noinst_LIBRARIES = libSear.a

SUFFIXES= .rc
INCLUDES = -I$(top_srcdir)

#AM_CPPFLAGS = $(BINRELOC_CFLAGS)

EXTRA_DIST = sear-res.rc

if BUILD_STATIC
sear_bin_LDFLAGS = -nodefaultlibs

#	/usr/lib/libSDL_mixer.a \
#	/usr/lib/libvorbisfile.a \
#	/usr/lib/libvorbis.a \
#	/usr/lib/libogg.a \
#	/usr/lib/libasound.a 

SEAR_EXT_LIBS = \
	/usr/lib/libSDL_image.a \
	/usr/lib/libtiff.a \
	/usr/lib/libjpeg.a \
	/usr/lib/libpng12.a \
	/usr/lib/libz.a \
	/usr/lib/libSDL.a \
	/usr/X11R6/lib/libX11.a \
	/usr/X11R6/lib/libXext.a \
	/usr/lib/libmercator-0.2.a \
	/usr/lib/libvarconf-1.0.a \
	/usr/lib/liberis-1.3.a \
	/usr/lib/libsigc-2.0.a \
	/usr/lib/libskstream-0.3.a \
	/usr/lib/libAtlasObjects-0.6.a \
	/usr/lib/libAtlasNet-0.6.a \
	/usr/lib/libAtlasCodecs-0.6.a \
	/usr/lib/libAtlasMessage-0.6.a \
	/usr/lib/libAtlas-0.6.a \
	/usr/lib/libwfmath-0.3.a \
	/usr/local/lib/libguichan_sdl.a \
	/usr/local/lib/libguichan_opengl.a \
	/usr/local/lib/libguichan.a \
	/usr/local/lib/libsage.a \
	/usr/local/lib/lib3ds.a \
	/usr/local/lib/libmd3-0.2.a \
	/usr/local/lib/libcal3d.a \
	/usr/X11R6/lib/libGLU.a \
	$(STATIC_LIBSTDCPP) \
	$(STATIC_LIBGCC) \
	/usr/lib/libGL.so \
	-lc -lm -lpthread -lgcc_s
else
SEAR_EXT_LIBS = $(SEAR_LIBS)
endif

if BUILD_RC_O
SEAR_RC_O=sear-res.o

else
SEAR_RC_O=
endif

#$(SEAR_RC_O): sear.rc
#	$(WINDRES) -i sear.rc -o $(SEAR_RC_O)

.rc.o:
	${WINDRES} $< -o $@

sear_bin_LDADD = \
	libSear.a \
	../renderers/libRendererGL.a \
	../environment/libEnvironment.a \
	../loaders/libModelLoader.a \
	../loaders/cal3d/libLoaderCal3d.a \
	../guichan/libGuichan.a \
	../Eris/libEris.a \
	../common/libCommon.a \
	$(SEAR_RC_O) \
	$(SEAR_EXT_LIBS)

sear_bin_SOURCES = \
	main.cpp

libSear_a_SOURCES = \
	ActionHandler.cpp ActionHandler.h \
	Bindings.cpp Bindings.h \
	binreloc.c binreloc.h \
	CacheManager.cpp CacheManager.h \
	CacheObject.cpp CacheObject.h \
	Calendar.cpp Calendar.h \
	Character.cpp Character.h \
	CharacterManager.cpp CharacterManager.h \
	client.cpp client.h \
	Console.cpp Console.h \
	ConsoleObject.h \
	Editor.cpp Editor.h \
	error.cpp error.h \
	Factory.cpp Factory.h \
	FileHandler.cpp FileHandler.h \
	MediaManager.cpp MediaManager.h \
	ScriptEngine.cpp ScriptEngine.h \
	System.cpp System.h \
	TerrainEntity.cpp TerrainEntity.h \
	WorldEntity.cpp WorldEntity.h \
	default_image.xpm \
	default_font.xpm \
	sear_icon.xpm

#	Sound.cpp \
#	Sound.h 

icondir = $(datadir)/icons/worldforge
icon_DATA = sear_icon.xpm
