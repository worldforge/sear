INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = model_viewer



if BUILD_STATIC
model_viewer_LDFLAGS = -nodefaultlibs

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


model_viewer_LDADD = \
        ../src/libSear.a \
        ../common/libCommon.a \
        ../renderers/libRendererGL.a \
        ../environment/libEnvironment.a \
        ../loaders/libModelLoader.a \
        ../loaders/cal3d/libLoaderCal3d.a \
        ../gui/libGui.a \
        ../guichan/libGuichan.a \
        $(SEAR_EXT_LIBS)

model_viewer_SOURCES = \
	model_viewer.cpp
