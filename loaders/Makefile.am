SUBDIRS = cal3d

LIB3DS_LIBS = libLoader3ds.a

noinst_LIBRARIES = libLoaderWireFrame.a libLoaderBoundBox.a libLoaderNPlane.a $(LIB3DS_LIBS)

INCLUDES = -I$(top_srcdir)

libLoaderWireFrame_a_SOURCES = \
	WireFrame.cpp WireFrame.h \
	WireFrame_Loader.cpp WireFrame_Loader.h

libLoaderBoundBox_a_SOURCES = \
	BoundBox.cpp BoundBox.h \
	BoundBox_Loader.cpp BoundBox_Loader.h

libLoader3ds_a_SOURCES = \
	3ds.cpp 3ds.h \
	3ds_Loader.cpp 3ds_Loader.h

libLoaderNPlane_a_SOURCES = \
	NPlane.cpp NPlane.h \
	NPlane_Loader.cpp NPlane_Loader.h
