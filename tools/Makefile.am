INCLUDES = -I..

bin_PROGRAMS = model_viewer

model_viewer_LDADD = \
	../renderers/libRendererGL.a \
	../common/libCommon.a \
	../loaders/libLoaderCal3d.a \
	../loaders/libLoaderBillBoard.a \
	../loaders/libLoaderBoundBox.a \
	../loaders/libLoaderImpostor.a \
	../loaders/libLoaderWireFrame.a

model_viewer_SOURCES = \
	model_viewer.cpp \
