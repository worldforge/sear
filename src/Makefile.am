bin_PROGRAMS = sear

noinst_LIBRARIES = libSear.a

INCLUDES = -I..

CAL3D_LIBS = ../loaders/libLoaderCal3d.a
LIB3DS_LIBS = ../loaders/libLoader3ds.a

sear_LDADD = \
	libSear.a \
	../common/libCommon.a \
	../renderers/libRendererGL.a \
	../terrain/libTerrainROAM.a \
	../sky/libSkyBox.a \
	../loaders/libLoaderWireFrame.a \
	../loaders/libLoaderBoundBox.a \
	../loaders/libLoaderNPlane.a \
	../loaders/libLoaderSlice.a \
	$(CAL3D_LIBS) \
	$(LIB3DS_LIBS) 

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
	ConsoleObject.h \
	cursors.h \
	Event.cpp \
	Event.h \
	EventHandler.cpp \
	EventHandler.h \
	Exception.h \
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
	Sky.h \
	System.cpp \
	System.h \
	Terrain.h \
	WorldEntity.cpp \
	WorldEntity.h \
	ModelHandler.cpp \
	ModelHandler.h \
	ModelLoader.h \
	StateLoader.cpp \
	StateLoader.h \
	Frustum.cpp \
	Frustum.h \
	FileHandler.cpp \
	FileHandler.h \
	default_image.xpm \
	default_font.xpm \
	ObjectRecord.h \
	ModelRecord.h \
	ObjectHandler.h \
	ObjectHandler.cpp
