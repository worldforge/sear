noinst_LIBRARIES = libCommon.a 

INCLUDES = -I$(top_srcdir)

if MMGR
MMGR_FILES=mmgr.cpp nommgr.h mmgr.h
else
MMGR_FILES=
endif

libCommon_a_SOURCES = \
	Log.cpp Log.h \
	Utility.cpp Utility.h \
	operations.h operations.cpp\
	types.h \
	$(MMGR_FILES)
