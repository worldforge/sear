#!/bin/sh -v
CROSSTOOLS=/home/sg02r/cross-tools
GCC=$CROSSTOOLS/i386-mingw32msvc/bin/gcc
WINDRES=$CROSSTOOLS/bin/i386-mingw32msvc-windres
STRIP=$CROSSTOOLS/i386-mingw32msvc/bin/strip

DIR=$CROSSTOOLS/lib
DIR2=$CROSSTOOLS/i386-mingw32msvc/lib

LIBS="-L $DIR -lSDL -lSDL_mixer -lSDL_image -lopengl32"

STAT=" -lcal3d -lvarconf -leris -lsigc -l3ds -lAtlasNet -lAtlasFunky -lAtlasObjectsEntity -lAtlasObjectsOperation -lAtlasObjects -lAtlasMessage -lAtlasCodecs -lAtlas -lskstream -lwsock32 -lwfmath -leris -lglu32 -lopengl32  -lm -leris "

$WINDRES -i sear.rc -o sear_rc.o

$GCC -o sear.exe src/*.o common/*.o loaders/*.o terrain/*.o sky/*.o renderers/*.o gui/*.o sear_rc.o $LIBS $STAT -lstdc++ $@

$STRIP sear.exe
