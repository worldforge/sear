SUFFIXES: .cxx .i .lo .la .cpp .o  .obj

.i.cxx:
	${SWIG} -Wall -DGCN_CORE_DECLSPEC -lua -c++ -I$(CPPFLAGS) -I$(top_srcdir) -o $@ $<

INCLUDES = -I${top_srcdir} 


noinst_LIBRARIES = libluaSear.a

libluaSear_a_SOURCES = \
	LuaScriptEngine.cpp LuaScriptEngine.h \
	LuaHelper.hpp \
	luasigc.hpp \
	Sear.cxx Guichan.cxx Eris.cxx

CLEANFILES = \
	Sear.cxx Guichan.cxx Eris.cxx

EXTRA_DIST = Sear.i Guichan.i Eris.i
