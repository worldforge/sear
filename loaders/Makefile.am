CAL3D_LIBS = libLoaderCal3d.a
LIB3DS_LIBS = libLoader3ds.a

noinst_LIBRARIES = libLoaderWireFrame.a libLoaderBoundBox.a libLoaderNPlane.a libLoaderSlice.a $(CAL3D_LIBS) $(LIB3DS_LIBS)

INCLUDES = -I..

libLoaderWireFrame_a_SOURCES = \
	WireFrame.cpp WireFrame.h \
	WireFrame_Loader.cpp WireFrame_Loader.h

libLoaderBoundBox_a_SOURCES = \
	BoundBox.cpp BoundBox.h \
	BoundBox_Loader.cpp BoundBox_Loader.h

libLoaderCal3d_a_SOURCES = \
	Cal3d.cpp Cal3d.h \
	Cal3d_Loader.cpp Cal3d_Loader.h

libLoader3ds_a_SOURCES = \
	3ds.cpp 3ds.h \
	3ds_Loader.cpp 3ds_Loader.h

libLoaderNPlane_a_SOURCES = \
	NPlane.cpp NPlane.h \
	NPlane_Loader.cpp NPlane_Loader.h


libLoaderSlice_a_SOURCES = \
	Slice.cpp Slice.h \
	Slice_Loader.cpp Slice_Loader.h
