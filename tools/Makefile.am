INCLUDES = -I..

bin_PROGRAMS = #model_viewer

model_viewer_LDADD = \
	../src/libSear.a \
	../terrain/libTerrainROAM.a \
	../sky/libSkyBox.a \
	../renderers/libRendererGL.a \
	../common/libCommon.a \
	../loaders/libLoader3ds.a \
	../loaders/libLoaderCal3d.a \
	../loaders/libLoaderBillBoard.a \
	../loaders/libLoaderBoundBox.a \
	../loaders/libLoaderImpostor.a \
	../loaders/libLoaderNPlane.a \
	../loaders/libLoaderWireFrame.a \
	../loaders/libLoaderSlice.a 

model_viewer_SOURCES = \
	model_viewer.cpp
