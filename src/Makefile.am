bin_PROGRAMS = sear

noinst_LIBRARIES = libSear.a

INCLUDES = -I$(top_srcdir)

EXTRA_DIST = sear.rc

CAL3D_LIBS = ../loaders/cal3d/libLoaderCal3d.a
LIB3DS_LIBS = ../loaders/libLoader3ds.a

if BUILD_RC_O
SEAR_RC_O=sear.rc.o

else
SEAR_RC_O=
endif

$(SEAR_RC_O): sear.rc
	$(WINDRES) -i sear.rc -o $(SEAR_RC_O)

sear_LDADD = \
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
	$(SEAR_RC_O)

sear_SOURCES = \
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
