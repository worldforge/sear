bin_PROGRAMS = sear-bin
bin_SCRIPTS = sear
noinst_LIBRARIES = libSear.a

INCLUDES = -I$(top_srcdir)

EXTRA_DIST = sear.rc

CAL3D_LIBS = ../loaders/cal3d/libLoaderCal3d.a
LIB3DS_LIBS = ../loaders/libLoader3ds.a

if BUILD_STATIC
sear_bin_LDFLAGS = -nodefaultlibs

SEAR_EXT_LIBS = \
	/usr/lib/libSDL_image.a \
	/usr/lib/libtiff.a \
	/usr/lib/libjpeg.a \
	/usr/lib/libpng12.a \
	/usr/lib/libz.a \
	/usr/lib/libSDL_mixer.a \
	/usr/lib/libvorbisfile.a \
	/usr/lib/libvorbis.a \
	/usr/lib/libogg.a \
	/usr/lib/libSDL.a \
	/usr/lib/libasound.a \
	/usr/X11R6/lib/libX11.a \
	/usr/X11R6/lib/libXext.a \
	/usr/lib/libmercator-0.2.a \
	/usr/lib/libvarconf-1.0.a \
	/usr/local/lib/liberis-1.2.a \
	/usr/lib/libsigc-2.0.a \
	/usr/lib/libskstream-0.3.a \
	/usr/lib/libAtlasObjectsOperation-0.5.a \
	/usr/lib/libAtlasObjectsEntity-0.5.a \
	/usr/lib/libAtlasObjects-0.5.a \
	/usr/lib/libAtlasNet-0.5.a \
	/usr/lib/libAtlasCodecs-0.5.a \
	/usr/lib/libAtlasMessage-0.5.a \
	/usr/lib/libAtlas-0.5.a \
	/usr/lib/libwfmath-0.3.a \
	/usr/local/lib/libsage.a \
	/usr/local/lib/lib3ds.a \
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
SEAR_RC_O=sear.rc.o

else
SEAR_RC_O=
endif

$(SEAR_RC_O): sear.rc
	$(WINDRES) -i sear.rc -o $(SEAR_RC_O)

sear_bin_LDADD = \
	libSear.a \
	../common/libCommon.a \
	../renderers/libRendererGL.a \
	../environment/libEnvironment.a \
	../loaders/libLoaderWireFrame.a \
	../loaders/libLoaderBoundBox.a \
	../loaders/libLoaderNPlane.a \
	../gui/libGui.a \
	$(CAL3D_LIBS) \
	$(LIB3DS_LIBS) \
	$(SEAR_RC_O) \
	$(SEAR_EXT_LIBS)

sear_bin_SOURCES = \
	main.cpp

libSear_a_SOURCES = \
	ActionHandler.cpp \
	ActionHandler.h \
	Bindings.cpp \
	Bindings.h \
	Camera.cpp \
	Camera.h \
	Character.cpp \
	Character.h \
	client.cpp \
	client.h \
	conf.h \
	Console.cpp \
	Console.h \
	cursors.h \
	Event.cpp \
	Event.h \
	EventHandler.cpp \
	EventHandler.h \
	Exception.h \
	Factory.cpp \
	Factory.h \
	Graphics.cpp \
	Graphics.h \
	Lobby.cpp \
	Lobby.h \
	main.cpp \
	Model.h \
	ModelStruct.h \
	Render.h \
	Sound.cpp \
	Sound.h \
	System.cpp \
	System.h \
	WorldEntity.cpp \
	WorldEntity.h \
	ModelHandler.cpp \
	ModelHandler.h \
	ModelLoader.h \
	Frustum.cpp \
	Frustum.h \
	FileHandler.cpp \
	FileHandler.h \
	default_image.xpm \
	default_font.xpm \
	sear_icon.xpm \
	ObjectRecord.h \
	ModelRecord.cpp \
	ModelRecord.h \
	ObjectHandler.h \
	ObjectHandler.cpp \
	ScriptEngine.cpp \
	ScriptEngine.h \
	Calendar.cpp \
	Calendar.h \
	Light.h \
	LightManager.h \
	LightManager.cpp \
	Editor.cpp \
	Editor.h

