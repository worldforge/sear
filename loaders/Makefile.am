if ENABLE_CAL3D
CAL3D_LIBS = libLoaderCal3d.a
else
CAL3D_LIBS = 
endif

if ENABLE_3DS
LIB3DS_LIBS = libLoader3ds.a
else
LIB3DS_LIBS = 
endif

noinst_LIBRARIES = libLoaderWireFrame.a libLoaderBoundBox.a libLoaderBillBoard.a libLoaderImpostor.a libLoaderNPlane.a $(CAL3D_LIBS) $(LIB3DS_LIBS)

INCLUDES = -I..

libLoaderWireFrame_a_SOURCES = \
	WireFrame.cpp WireFrame.h \
	WireFrame_Loader.cpp WireFrame_Loader.h

libLoaderBoundBox_a_SOURCES = \
	BoundBox.cpp BoundBox.h \
	BoundBox_Loader.cpp BoundBox_Loader.h

libLoaderBillBoard_a_SOURCES = \
	BillBoard.cpp BillBoard.h \
	BillBoard_Loader.cpp BillBoard_Loader.h

libLoaderImpostor_a_SOURCES = \
	Impostor.cpp Impostor.h \
	Impostor_Loader.cpp Impostor_Loader.h

libLoaderCal3d_a_SOURCES = \
	Cal3d.cpp Cal3d.h \
	Cal3d_Loader.cpp Cal3d_Loader.h

libLoader3ds_a_SOURCES = \
	3ds.cpp 3ds.h \
	3ds_Loader.cpp 3ds_Loader.h

libLoaderNPlane_a_SOURCES = \
	NPlane.cpp NPlane.h \
	NPlane_Loader.cpp NPlane_Loader.h
