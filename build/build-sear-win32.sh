#!/bin/bash
DATE=`date`
echo "Build Started $DATE"

setenvironment() {

HOST_CC="/usr/bin/gcc"
export HOST_CC

XCC_INSTALL=/usr/local/mingw/3.9/
export XCC_INSTALL


export SRCDIR=$HOME/build_roots/sear-win32/src

PREFIX=$HOME/build_roots/sear-win32/
export PREFIX

ACLOCAL_FLAGS="-I $PREFIX/share/aclocal -I $XCC_INSTALL/i386-mingw32msvc/share/aclocal"
export ACLOCAL_FLAGS

export PKG_CONFIG_LIBDIR="$PREFIX/lib/pkgconfig"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig"


TARGET=i386-mingw32msvc
export TARGET

DLLTOOL=$XCC_INSTALL/bin/$TARGET-dlltool
export DLLTOOL

CC_FOR_BUILD="$XCC_INSTALL/bin/$TARGET-gcc"
export CC_FOR_BUILD


STRIP=$XCC_INSTALL/bin/$TARGET-strip

cache=cross-config.cache

CONFIGURE="./configure --target=$TARGET --host=$TARGET --build=i386-linux"
export CONFIGURE

MAKE=make
export MAKE

PATH="$XCC_INSTALL/bin:$PATH"
export PATH


export CFLAGS="-DCURL_STATICLIB=1"
export CXXFLAGS=$CFLAGS

export JOBS=-j2

export WFUT_ROOT=$HOME/build_roots/WFUT/
}


build_sigc() {
  pushd $SRCDIR/libsigc++-2.0.16
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_guichan() {
  pushd $SRCDIR/guichan-0.8.1
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared  --enable-force-opengl
  cat > include/guichan/platform.hpp << EOF
#ifndef GCN_PLATFORM_HPP
#define GCN_PLATFORM_HPP

#ifndef GCN_CORE_DECLSPEC
#define GCN_CORE_DECLSPEC
#endif

#ifndef GCN_EXTENSION_DECLSPEC
#define GCN_EXTENSION_DECLSPEC
#endif

#ifndef NULL
#define NULL 0
#endif

#endif // end GCN_PLATFORM_HPP
EOF

  $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_guichan_7() {
  pushd $SRCDIR/guichan-0.7.1
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared  --enable-force-opengl
  cat > include/guichan/platform.hpp << EOF
#ifndef GCN_PLATFORM_HPP
#define GCN_PLATFORM_HPP

#ifndef GCN_CORE_DECLSPEC
#define GCN_CORE_DECLSPEC
#endif

#ifndef GCN_EXTENSION_DECLSPEC
#define GCN_EXTENSION_DECLSPEC
#endif

#ifndef NULL
#define NULL 0
#endif

#endif // end GCN_PLATFORM_HPP
EOF

  $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_guichan_6() {
  pushd $SRCDIR/guichan-0.6.1
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared  --enable-force-opengl && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_guichan_5() {
  pushd $SRCDIR/guichan-0.5.0-src
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared  --enable-force-opengl && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_cal3d() {
  pushd $SRCDIR/cal3d-0.11.0
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}
build_curl() {
  pushd $SRCDIR/curl-7.16.1
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}


build_lib3ds() {
  pushd $SRCDIR/lib3ds-1.3.0
  make clean
  export PATH=$XCC_INSTALL/i386-mingw32msvc/bin:$PATH
  which gcc
echo "WARNING: mingw binaries need to be on the path (gcc, ranlib, ar etc)"
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}
build_libmodelfile() {
  pushd $SRCDIR/libmodelfile-0.1.92
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}
build_sage_cvs() {
  pushd $SRCDIR/forge/libs/sage/
  make distclean
  cvs up -d
  ./autogen.sh
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_atlas_cvs() {
  pushd $SRCDIR/forge/libs/Atlas-C++
  make distclean
  cvs up -d
  ./autogen.sh
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}
build_wfmath_cvs() {
  pushd $SRCDIR/forge/libs/wfmath 
  make distclean
  cvs up -d
  ./autogen.sh &&  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_varconf_cvs() {
  pushd $SRCDIR/forge/libs/varconf
  make distclean
  cvs up -d
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_skstream_cvs() {
  pushd $SRCDIR/forge/libs/skstream 
  make distclean
  cvs up -d 
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --disable-shared --enable-getaddrinfo=no && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $?
}
build_mercator_cvs() {
  pushd $SRCDIR/forge/libs/mercator
  make distclean
  cvs up -d 
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $?
}

build_libwfut_cvs() {
  pushd $SRCDIR/libwfut
  make distclean
  git pull 
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  ## Manual build of wfut.exe to force in a static zlib
  pushd tools
  i386-mingw32msvc-g++ -o wfut.exe wfut.cpp `pkg-config libwfut-0.2 --libs --cflags` -I ../ -DHAVE_CONFIG_H -lwsock32  ~/build_roots/fvs-win32/lib/libz.a -lwinmm
  popd
  $STRIP tools/wfut.exe && cp tools/wfut.exe $WFUT_ROOT/sear-devel-win32/ && $HOME/bin/updateWFUT
  popd
  return $RET
}

build_eris_cvs() {
  pushd $SRCDIR/forge/libs/eris
  make distclean
  cvs up -d && ./autogen.sh && $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_cyphesis_cvs() {
  pushd $SRCDIR/forge/servers/cyphesis-C++
  make distclean  
  cvs up -d
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS
  RET=$?
  popd
  return $RET
}

build_sear_cvs(){
  pushd $SRCDIR/sear
  cvs up -d 
  unix2dos -n README readme.txt
  unix2dos -n NEWS NEWS.txt
  date > BuildStamp && ./autogen.sh
  $CONFIGURE --prefix=$PREFIX --with-guichan-includes=$PREFIX/includes --with-guichan-libraries=$PREFIX/lib --enable-debug --disable-binreloc && $MAKE $JOBS && $STRIP src/sear-bin.exe && cp BuildStamp readme.txt NEWS.txt src/sear-bin.exe $WFUT_ROOT/sear-devel-win32/ #  && $HOME/bin/updateWFUT
  RET=$?
  popd
  return $RET
}

build_sear_git(){
  pushd $SRCDIR/sear
  git pull origin
  unix2dos -n README readme.txt
  unix2dos -n NEWS NEWS.txt
  date > BuildStamp && ./autogen.sh
  $CONFIGURE --prefix=$PREFIX --with-guichan-includes=$PREFIX/includes --with-guichan-libraries=$PREFIX/lib --enable-debug --disable-binreloc && $MAKE $JOBS && $STRIP src/sear-bin.exe && cp BuildStamp readme.txt NEWS.txt src/sear-bin.exe $WFUT_ROOT/sear-devel-win32/ #  && $HOME/bin/updateWFUT
  RET=$?
  popd
  return $RET
}

setenvironment
case $1 in 
  all)
  build_libwfut_cvs && build_atlas_cvs && build_wfmath_cvs && build_mercator_cvs && build_varconf_cvs && build_skstream_cvs && build_eris_cvs && build_sage_cvs && build_sear_git
  RET=$?
  ;;
  cyphesis_cvs)
  build_cyphesis_cvs
  RET=$?
  ;;
  atlas_cvs)
  build_atlas_cvs
  RET=$?
  ;;
  sear)
  build_sear_git
  RET=$?
  ;;
  skstream)
  build_skstream_cvs
  RET=$?
  ;;
  sage)
  build_sage_cvs
  RET=$?
  ;;
  guichan_6)
  build_guichan_6
  RET=$?
  ;;
  guichan)
  build_guichan
  RET=$?
  ;;
  libwfut)
  build_libwfut_cvs
  RET=$?
  ;;
  lib3ds)
  build_lib3ds
  RET=$?
  ;;
  libmodelfile)
  build_libmodelfile
  RET=$?
  ;;
  sigc)
  build_sigc
  RET=$?
  ;;
  bash)
  bash
  RET=$?
  ;;
  curl)
  build_curl
  RET=$?
  ;;
  cal3d)
  build_cal3d
  RET=$?
  ;;
  *)
  echo "Usage $0 all|sear"
  RET=1
esac
exit $RET
