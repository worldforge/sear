#!/bin/bash
DATE=`date`
echo "Build Started $DATE"

setenvironment() {

export PREFIX=$HOME/build_roots/sear-linux
export ACLOCAL_FLAGS="-I $PREFIX/share/aclocal"
export PKG_CONFIG_LIBDIR="$PREFIX/lib/pkgconfig"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig"
export PATH="$PREFIX/bin:$PATH"
export SRCDIR=$PREFIX/src
export STRIP=/usr/bin/strip
export MAKE=make
export CONFIGURE=./configure
export LDFLAGS="-L$PREFIX/lib"
export CC=/usr/bin/gcc34
export CXX=/usr/bin/g++34

#CC=$HOME/bin/apgcc
#CXX=$HOME/bin/apg++
#export CC CXX

export JOBS=-j2
export WFUT_ROOT=$HOME/build_roots/WFUT/

}

build_sdl_image() {
  pushd $SRCDIR/SDL_image-1.2.6
  make clean
  rm autom4te.cache -rf
  $CONFIGURE --prefix=$PREFIX --disable-shared --enable-static && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}


build_sdl() {
  pushd $SRCDIR/SDL-1.2.12
  make clean
  rm autom4te.cache -rf
  $CONFIGURE --prefix=$PREFIX --disable-shared --enable-static && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_dbus() {
  pushd $SRCDIR/dbus-1.0.2
  make clean
  rm autom4te.cache -rf
  $CONFIGURE --prefix=$PREFIX --disable-shared --with-system-pid-file=/var/run/messagebus.pid  --with-system-socket=/var/run/dbus/system_bus_socket     && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_avahi() {
  pushd $SRCDIR/avahi-0.6.18
  make clean
  rm autom4te.cache -rf
  $CONFIGURE --prefix=$PREFIX --disable-shared --disable-qt3 --disable-qt4 --disable-gtk --disable-glib --disable-mono --disable-monodoc --disable-autoipd --disable-python-dbus --disable-expat --disable-libdaemon --disable-python  --disable-pygtk && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_tiff() {
  pushd $SRCDIR/tiff-v3.5.7
  make clean
## Build config options
  cat << EOF > config.site
JPEG="yes"
ZIP="yes"
DIR_BIN="$PREFIX/bin"
DIR_LIB="$PREFIX/lib"
DIR_INC="$PREFIX/include"
DIR_MAN="$PREFIX/man"
DIRS_LIBINC="$DIRS_LIBINC $PREFIX/include"
DIR_JPEGLIB=$PREFIX/lib
DIR_GZLIB=$PREFIX/lib
EOF
  $CONFIGURE --noninteractive --prefix=$PREFIX
  $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_jpeg() {
  pushd $SRCDIR/jpeg-6b
  make clean
  $CONFIGURE  --enable-static --disable-shared --prefix=$PREFIX
  $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_libwfut_cvs() {
  pushd $SRCDIR/forge/libs/libwfut 
  $MAKE distclean
  cvs up -d && ./autogen.sh && $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE $JOBS && $MAKE install
  RET=$?
  pushd tools
  $CXX -o wfut wfut.cpp `pkg-config libwfut-0.1 --libs --cflags` -I ../ -DHAVE_CONFIG_H /usr/lib/libz.a -ldl
  popd
  $STRIP tools/wfut && cp tools/wfut $WFUT_ROOT/sear-devel-linux/bin/
  popd
  return $RET
}

build_libmodelfile() {
  pushd $SRCDIR/libmodelfile-0.1.92
  make clean
  rm autom4te.cache -rf
  $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_lib3ds() {
  pushd $SRCDIR/lib3ds-1.3.0
  make clean
  rm autom4te.cache -rf
  rm config.cache -rf
  $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_guichan() {
  pushd $SRCDIR/guichan-0.8.1
  make clean
  rm autom4te.cache -rf
  $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_guichan_6() {
  cd $SRCDIR/guichan-0.6.1 &&  rm autom4te.cache -rf &&$CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  return $?
}

build_guichan_5() {
  cd $SRCDIR/guichan-0.5.0-src &&  rm autom4te.cache -rf &&$CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  return $?
}

build_cal3d() {
  pushd $SRCDIR/cal3d-0.11.0
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared --enable-static && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_curl() {
  pushd $SRCDIR/curl-7.16.1
  make clean
  $CONFIGURE --prefix=$PREFIX --disable-shared --without-ssl --without-libssh2 --without-ca-bundle  --without-libidn && $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

build_skstream() {
  pushd $SRCDIR/forge/libs/skstream 
  make distclean
  cvs up -d 
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_atlas() {
  pushd $SRCDIR/forge/libs/Atlas-C++
  make distclean
  cvs up -d 
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_wfmath() {
  pushd $SRCDIR/forge/libs/wfmath
  make distclean
  cvs up -d
  ./autogen.sh &&  $CONFIGURE --disable-shared --enable-static --prefix=$PREFIX && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_varconf() {
  pushd $SRCDIR/forge/libs/varconf
  make distclean
  cvs up -d 
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_mercator() {
  pushd $SRCDIR/forge/libs/mercator
  make distclean
  cvs up -d
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_eris() {
  pushd $SRCDIR/forge/libs/eris
  make distclean
  cvs up -d
  ./autogen.sh && $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_sear(){
  pushd $SRCDIR/sear
  make distclean
  git pull origin
  rm autom4te.cache -rf
  date > BuildStamp && ./autogen.sh && $CONFIGURE --with-guichan-includes=$PREFIX/includes --with-guichan-libraries=$PREFIX/lib --enable-static-build --enable-binreloc && $MAKE -j2 && $STRIP src/sear-bin && cp BuildStamp README NEWS src/sear-bin $WFUT_ROOT/sear-devel-linux/bin/ # && $HOME/bin/updateWFUT
  RET=$?
  popd
  return $?
}

build_sear_cvs(){
  pushd $SRCDIR/forge/clients/sear
  make distclean
  cvs up -d 
  rm autom4te.cache -rf
  date > BuildStamp && ./autogen.sh && $CONFIGURE --with-guichan-includes=$PREFIX/includes --with-guichan-libraries=$PREFIX/lib --enable-static-build --enable-binreloc && $MAKE -j2 && $STRIP src/sear-bin && cp BuildStamp README NEWS src/sear-bin $WFUT_ROOT/sear-devel-linux/bin/ # && $HOME/bin/updateWFUT
  RET=$?
  popd
  return $?
}


build_sage_cvs() {
  pushd $SRCDIR/forge/libs/sage/
  make distclean
  cvs up -d
  ./autogen.sh
  $CONFIGURE --prefix=$PREFIX --disable-shared && $MAKE -j2 && $MAKE install
  RET=$?
  popd
  return $RET
}

build_sigc() {
  pushd $SRCDIR/libsigc++-2.0.16
  $MAKE distclean
  $CONFIGURE --prefix=$PREFIX --enable-static --disable-shared
  $MAKE $JOBS && $MAKE install
  RET=$?
  popd
  return $RET
}

setenvironment
case $1 in 
  rebuild)
  build_curl && build_sigc && build_dbus && build_avahi && build_tiff && build_jpeg && build_cal3d && build_guichan && build_lib3ds && build_libmodelfile && build_atlas  && build_skstream && build_wfmath && build_varconf && build_mercator && build_libwfut_cvs && build_sage_cvs && build_eris && build_sear
  RET=$?
  ;;
  all)
  build_libwfut_cvs && build_sage_cvs && build_atlas && build_wfmath && build_mercator && build_eris && build_varconf && build_sear
  RET=$?
  ;;
  sage)
  build_sage_cvs
  RET=$?
  ;;
  lib3ds)
  build_lib3ds
  RET=$?
  ;;
  guichan)
  build_guichan
  RET=$?
  ;;
  skstream)
  build_skstream
  RET=$?
  ;;
  libmodelfile)
  build_libmodelfile
  RET=$?
  ;;
  sdl_image)
  build_sdl_image
  RET=$?
  ;;
  sdl)
  build_sdl
  RET=$?
  ;;
  sigc)
  build_eris
  RET=$?
  ;;
  sigc)
  build_sigc
  RET=$?
  ;;
  curl)
  build_curl
  RET=$?
  ;;
  libwfut)
  build_libwfut_cvs
  RET=$?
  ;;
  sear)
  build_sear
  RET=$?
  ;;
  dbus)
  build_dbus
  RET=$?
  ;;
  avahi)
  build_avahi
  RET=$?
  ;;
  bash)
  bash
  RET=$?
  ;;
  wfut)
  $HOME/bin/updateWFUT
  RET=$?
  ;;
  jpeg)
  build_jpeg
  RET=$?
  ;;
  tiff)
  build_tiff
  RET=$?
  ;;
  *)
  echo "Usage $0 sear|all"
  exit 1;
esac
exit $RET
