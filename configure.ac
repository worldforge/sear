dnl Initialise autoconf

AC_INIT()
AM_CONFIG_HEADER(config.h)

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Initialise automake
SEAR_MAJOR_VERSION=0
SEAR_MINOR_VERSION=5
SEAR_MICRO_VERSION=0
SEAR_VERSION=$SEAR_MAJOR_VERSION.$SEAR_MINOR_VERSION.$SEAR_MICRO_VERSION

AC_SUBST(SEAR_VERSION)

AC_ENABLE_STATIC(yes)
AC_ENABLE_SHARED(no)

AM_INIT_AUTOMAKE(sear, $SEAR_VERSION)

AC_DISABLE_FAST_INSTALL

AC_PROG_LIBTOOL
dnl Need libtool


#AM_PROG_LIBTOOL
dnl AC_PROG_LIBTOOL

dnl Test for C++ compiler

AC_PROG_CXX

AC_LANG_CPLUSPLUS
AC_REQUIRE_CPP

AC_HEADER_STDC
CXXFLAGS="-ansi -pedantic -Wall"
CPPFLAGS=""
CFLAGS=""
AC_ARG_ENABLE(debug,
  [  --enable-debug	enable debug information [default=no]],
  [
    if test "$enableval" = "yes"; then
      CXXFLAGS="$CXXFLAGS -ggdb -O0 -DDEBUG"
    else
      CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG"
    fi
  ],[
    CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG"
  ]
)


dnl Check SDL
SDL_VERSION=1.2.2
AM_PATH_SDL($SDL_VERSION, [
  CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
  LIBS="$LIBS $SDL_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find SDL or insufficent version found. Please goto http://www.libsdl.org/ and get at least version $SDL_VERSION)
])


SIGC_VERSION=1.0.3
PKG_CHECK_MODULES(SIGC, sigc++-1.2 >= 1.1.11,
    [
        CPPFLAGS="$CPPFLAGS $SIGC_CFLAGS"
        LDFLAGS="$LDFLAGS $SIGC_LIBS"
    ],
    [
        AM_PATH_SIGC($SIGC_VERSION,[
                CPPFLAGS="$CPPFLAGS $SIGC_CFLAGS"
                LDFLAGS="$LDFLAGS $SIGC_LIBS"
                ],
                AC_MSG_ERROR(Couldn't find sigc++ or insufficient version found. Please goto http://sourceforge.net/projects/libsigc/ and get at least version $SIGC_VERSION)
        )
    ])


AC_CHECK_LIB(lua, lua_open, [
  LUA_LIBS="-llua -llualib"
  AC_DEFINE(HAVE_LUA, 1, [Found lua library])
  AC_SUBST(LUA_LIBS)
],[
  AC_MSG_ERROR([lua not found. You can download it from www.lua.org])
])

dnl Check Atlas
ATLAS_VERSION=0.4.5
AM_PATH_ATLAS($ATLAS_VERSION, [
  CPPFLAGS="$CPPFLAGS $ATLAS_CFLAGS"
  LIBS="$LIBS $ATLAS_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find Atlas-C++ or wrong version found. Please goto ftp://ftp.worldforge.org/pub/worldforge/libs/Atlas-C++ and get at least version $ATLAS_VERSION)
])

dnl Check WFMath
WFMATH_VERSION=0.2.10
AM_PATH_WFMATH($WFMATH_VERSION, [
  CPPFLAGS="$CPPFLAGS $WFMATH_CFLAGS"
  LIBS="$LIBS $WFMATH_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find WFMath or insufficent version found. Please get the latest version from WorldForge CVS.)
])


dnl Check varconf
VARCONF_VERSION=0.5.4
AM_PATH_VARCONF($VARCONF_VERSION, [
  CPPFLAGS="$CPPFLAGS $VARCONF_CFLAGS"
  LIBS="$LIBS $VARCONF_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find VarConf or wrong version found. Please goto ftp://ftp.worldforge.org/pub/worldforge/libs/varconf and get at least version $VARCONF_VERSION)
])


dnl Check Eris
ERIS_VERSION=0.9.7
AM_PATH_ERIS($ERIS_VERSION, [
  CPPFLAGS="$CPPFLAGS $ERIS_CFLAGS"
  LIBS="$LIBS $ERIS_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find Eris or insufficent version found. Please get the latest version from WorldForge CVS.)
])



if test "x$prefix" != xNONE; then
    PREFIX=$prefix
  else
    PREFIX=$ac_default_prefix
fi
AC_DEFINE_UNQUOTED(INSTALLDIR,"$PREFIX",[Install prefix])

AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

dnl Generate files
AC_OUTPUT([
	sear.dox
	Makefile
	interfaces/Makefile
	script/Makefile
	engine/Makefile
	src/Makefile
])
