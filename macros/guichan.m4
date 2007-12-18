dnl Taken from vips
dnl From FIND_MOTIF and ACX_PTHREAD, without much understanding
dnl
dnl FIND_GUICHAN[ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]]
dnl ------------------------------------------------
dnl
dnl Find GUICHAN libraries and headers
dnl
dnl Put compile stuff in GUICHAN_INCLUDES
dnl Put link stuff in GUICHAN_LIBS
dnl
dnl Default ACTION-IF-FOUND defines HAVE_GUICHAN
dnl
AC_DEFUN([FIND_GUICHAN], [

GUICHAN_INCLUDES=""
GUICHAN_LIBS=""

AC_ARG_WITH(guichan, 
[  --without-guichan		do not use libguichan])
# Treat --without-guichan like --without-guichan-includes --without-guichan-libraries.
if test "$with_guichan" = "no"; then
	GUICHAN_INCLUDES=no
	GUICHAN_LIBS=no
fi

AC_ARG_WITH(guichan-includes,
[  --with-guichan-includes=DIR	GUICHAN include files are in DIR],
GUICHAN_INCLUDES="-I$withval")
AC_ARG_WITH(guichan-libraries,
[  --with-guichan-libraries=DIR	GUICHAN libraries are in DIR],
GUICHAN_LIBS="-L$withval -lguichan_sdl -lguichan_opengl -lguichan")

AC_MSG_CHECKING(for GUICHAN)

# Look for Guichan.h 
if test "$GUICHAN_INCLUDES" = ""; then
	Guichan_save_LIBS="$LIBS"

	LIBS="$LIBS -lguichan_sdl -lguichan_opengl -lguichan"

	# Check the standard search path
	AC_TRY_COMPILE([
		#include <guichan.hpp>],[int a],[
		GUICHAN_INCLUDES=""
	], [
		# guichan.h is not in the standard search path.

		# A whole bunch of guesses
		for dir in \
			"${prefix}"/*/include \
			/usr/local/include \
			/usr/*/include \
			/usr/local/*/include /usr/*/include \
			"${prefix}"/include/* \
			/usr/include/* /usr/local/include/* /*/include; do
			if test -f "$dir/guichan.hpp"; then
				GUICHAN_INCLUDES="-I$dir"
				break
			fi
		done

		if test "$GUICHAN_INCLUDES" = ""; then
			GUICHAN_INCLUDES=no
		fi
	])

	LIBS="$Guichan_save_LIBS"
fi

# Now for the libraries
if test "$GUICHAN_LIBS" = ""; then
	Guichan_save_LIBS="$LIBS"
	Guichan_save_INCLUDES="$INCLUDES"

	LIBS=" -lguichan_sdl -lguichan_opengl -lguichan $LIBS"
	INCLUDES="$GUICHAN_INCLUDES $INCLUDES"

	# Try the standard search path first
	AC_TRY_LINK([
		#include <stdio.h>
		#include <guichan.hpp>],[gcnGuichanVersion], [
		GUICHAN_LIBS=" -lguichan_sdl -lguichan_opengl -lguichan"
	], [
		# libGuichan is not in the standard search path.

		# A whole bunch of guesses
		for dir in \
			"${prefix}"/*/lib \
			"${prefix}"/*/lib64 \
			/usr/local/lib \
			/usr/local/lib64 \
			/usr/*/lib \
			/usr/*/lib64 \
			/usr/lib64 \
			"${prefix}"/lib/* /usr/lib/* \
			"${prefix}"/lib64/* /usr/lib64/* \
			/usr/local/lib/* /*/lib; do
			if test -d "$dir" && test "`ls $dir/libguichan.* 2> /dev/null`" != ""; then
				GUICHAN_LIBS="-L$dir  -lguichan_sdl -lguichan_opengl -lguichan"
				break
			fi
		done

		if test "$GUICHAN_LIBS" = ""; then
			GUICHAN_LIBS=no
		fi
	])

	LIBS="$Guichan_save_LIBS"
	INCLUDES="$Guichan_save_INCLUDES"
fi

AC_SUBST(GUICHAN_LIBS)
AC_SUBST(GUICHAN_INCLUDES)

# Print a helpful message
Guichan_libraries_result="$GUICHAN_LIBS"
Guichan_includes_result="$GUICHAN_INCLUDES"

if test x"$Guichan_libraries_result" = x""; then
	Guichan_libraries_result="in default path"
fi
if test x"$Guichan_includes_result" = x""; then
	Guichan_includes_result="in default path"
fi

if test "$Guichan_libraries_result" = "no"; then
	Guichan_libraries_result="(none)"
fi
if test "$Guichan_includes_result" = "no"; then
	Guichan_includes_result="(none)"
fi

AC_MSG_RESULT(
  [libraries $Guichan_libraries_result, headers $Guichan_includes_result])

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test "$GUICHAN_INCLUDES" != "no" && test "$GUICHAN_LIBS" != "no"; then
        ifelse([$1],,AC_DEFINE(HAVE_GUICHAN,1,[Define if you have Guichan libraries and header files.]),[$1])
        :
else
	GUICHAN_INCLUDES=""
	GUICHAN_LIBS=""
        $2
fi

AM_CONDITIONAL(HAVE_GUICHAN, test "$GUICHAN_LIBS" != "no")
])dnl
