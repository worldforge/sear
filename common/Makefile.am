noinst_LIBRARIES = libCommon.a 

INCLUDES = -I..

if MMGR
MMGR_FILES=mmgr.cpp nommgr.h mmgr.h
else
MMGR_FILES=
endif

libCommon_a_SOURCES = \
	Log.cpp Log.h \
	Utility.cpp Utility.h \
	types.h \
	$(MMGR_FILES)
