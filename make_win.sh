#!/bin/sh -v
GCC=/home/sg02r/cross-tools/i386-mingw32msvc/bin/gcc
STRIP=/home/sg02r/cross-tools/i386-mingw32msvc/bin/strip

DIR=/home/sg02r/cross-tools/lib
DIR2=/home/sg02r/cross-tools/i386-mingw32msvc/lib

LIBS="-L /home/sg02r/cross-tools/lib -lSDL -lSDL_mixer -lSDL_image -lopengl32"

STAT=" -lcal3d -lvarconf -leris -lsigc -l3ds -lAtlasNet -lAtlasFunky -lAtlasObjectsEntity -lAtlasObjectsOperation -lAtlasObjects -lAtlasMessage -lAtlasCodecs -lAtlas -lskstream -lwsock32 -lwfmath -leris -lglu32 -lopengl32  -lm -leris "

$GCC -o sear.exe src/*.o common/*.o loaders/*.o terrain/*.o sky/*.o renderers/*.o gui/*.o $LIBS $STAT -lstdc++

$STRIP sear.exe
