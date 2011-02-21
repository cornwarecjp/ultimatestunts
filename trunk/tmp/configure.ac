dnl Process this file with autoconf to produce a configure script.

AC_INIT([Ultimate Stunts], 0.7.6, [cornware-cjp@users.sourceforge.net], [ultimatestunts])
AC_CONFIG_SRCDIR([ultimatestunts/main.cpp])
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_USE_SYSTEM_EXTENSIONS

PKG_PROG_PKG_CONFIG

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_EXEEXT
AC_OBJEXT

AC_LANG(C++)


dnl ----------------------
dnl Initially no libraries
dnl ----------------------
graphlibs=""
netlibs=""
soundlibs=""
sdllibs=""

dnl ----------------------------------
dnl check for OpenGL related libraries
dnl this comes from Tuxkart
dnl ----------------------------------
AC_CHECK_HEADER(windows.h)

if test "x$ac_cv_header_windows_h" != "xyes" ; then

  dnl Reasonable stuff non-windoze variants ... :-)

  if test "x`uname -s`" = "xDarwin"; then
    dnl On mac OS X flags are a bit different
    graphlibs="-framework OpenGL $graphlibs"
  else
    AC_CHECK_LIB(GL, glNewList, graphlibs="-lGL $graphlibs" ,,$graphlibs)
    if test "x$ac_cv_lib_GL_glNewList" = "xno" ; then
      dnl if no GL, check for MesaGL
      AC_CHECK_LIB(MesaGL, glNewList,graphlibs="-lMesaGL $graphlibs",,$graphlibs)
    fi
  fi

  dnl if using mesa, check for xmesa.h
  if test "x$ac_cv_lib_MesaGL_glNewList" = "xyes" ; then
    AC_CHECK_HEADER(GL/xmesa.h)
    AM_CONDITIONAL(ENABLE_XMESA_FX, \
                test "x$ac_cv_header_GL_xmesa_h" = "xyes")
  else
    dnl force a failed check
    AM_CONDITIONAL(ENABLE_XMESA_FX, test "no" = "yes")
  fi

else
  dnl CygWin under Windoze.

  graphlibs="-lm -lopengl32"
  CFLAGS="$CFLAGS -I/usr/include/opengl"
  CXXFLAGS="$CXXFLAGS -I/usr/include/opengl"

  dnl force a failed check
  AM_CONDITIONAL(ENABLE_XMESA_FX, test "no" = "yes")
fi

dnl -------------
dnl Check for SDL
dnl -------------
AC_LANG_PUSH(C)
AM_PATH_SDL(1.1.0, sdllibs="$SDL_LIBS $sdllibs";AC_DEFINE([HAVE_SDL], [], [SDL Lib]))
AC_LANG_POP(C)
CFLAGS="$CFLAGS $SDL_CFLAGS"
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"

dnl -------------
dnl Check for SDL_image
dnl -------------
haveSDLimage="yes"
AC_CHECK_HEADER(SDL_image.h, AC_DEFINE([SDL_IMAGE_HEADER], [], [SDL_image header]))
AC_CHECK_LIB(SDL_image, IMG_Load, graphlibs="-lSDL_image $graphlibs";AC_DEFINE([HAVE_SDL_IMAGE], [], [SDL_image Lib]), [haveSDLimage="no"])


dnl -------------
dnl added "-mconsole" for stdio in windows
dnl -------------
if test "x$ac_cv_header_windows_h" = "xyes" ; then
  sdllibs="$sdllibs -mconsole"
fi

dnl ---------------------
dnl Check sockets library
dnl ---------------------
AC_CHECK_LIB(socket, socket,netlibs="-lsocket $netlibs")
AC_CHECK_LIB(nsl, gethostbyname,netlibs="-lnsl $netlibs")
AC_CHECK_LIB(resolv, inet_aton,netlibs="-lresolv $netlibs")


dnl -----------------------
dnl Checks for header files
dnl -----------------------
AC_HEADER_STDC

AC_CHECK_HEADER(GL/gl.h)
AC_CHECK_HEADER(GL/glext.h,AC_DEFINE([HAVE_GLEXT_H], [], [glext.h header]),[], [#include <GL/gl.h>])

try_openal="yes"
try_fmod="yes"

AC_ARG_WITH([openal],
    AC_HELP_STRING([--with-openal], [Only use OpenAL (don't try to use FMOD)]),
    [try_fmod="no"], [])
AC_ARG_WITH([fmod],
    AC_HELP_STRING([--with-fmod], [Only use FMOD (don't try to use OpenAL)]),
    [try_openal="no"], [])

if test "$try_openal" = "yes" ; then
  dnl ------------------------
  dnl Check openAL audio library
  dnl ------------------------
  AC_CHECK_HEADER(AL/al.h,
      found_al_header="yes";AC_DEFINE([OPENAL_HEADER], [], [OpenAL headers]),
      found_al_header="no")

  AC_CHECK_HEADER(AL/alut.h,
      found_alut_header="yes";AC_DEFINE([ALUT_HEADER], [], [ALUT headers]),
      found_alut_header="no")

  AC_CHECK_LIB(openal, alGetError,
      found_libopenal="yes";AC_DEFINE([HAVE_LIBOPENAL], [], [OpenAL Lib]),
      found_libopenal="no")

  if test "$found_al_header $found_alut_header $found_libopenal" = "yes yes yes"; then

      AC_CHECK_LIB(alut, alutExit,
          soundlibs="-lalut -lopenal $soundlibs";AC_DEFINE([HAVE_LIBALUT], [], [ALUT Lib]),
          [],
          [-lopenal])

      dnl Now, if soundlibs is not empty, then both AL and ALUT are completely found

      if test "x$soundlibs" != "x" ; then

        dnl Don't try FMOD when OpenAL is found
        try_fmod="no"

        dnl When using openAL, also try to use vorbisfile
        PKG_CHECK_MODULES([vorbisfile], [vorbisfile >= 1.0.0],
            [AC_DEFINE([HAVE_VORBISFILE], [], [Vorbisfile Lib])])
        soundlibs="$vorbisfile_LIBS $soundlibs"
        CFLAGS="$vorbisfile_CFLAGS $CFLAGS"
        CXXFLAGS="$vorbisfile_CFLAGS $CXXFLAGS"
      fi
  fi
fi

if test "$try_fmod" = "yes" ; then
  dnl ------------------------
  dnl Check fmod audio library
  dnl ------------------------
  AC_CHECK_HEADER(fmod/fmod.h, AC_DEFINE([FMOD_HEADER], [], [FMOD headers]))

  dnl OS X needs an extra flag
  if test "x`uname -s`" = "xDarwin"; then
    fmodlibs="-lfmod -framework Carbon"
  else
    fmodlibs="-lfmod"
  fi

  dnl This doesn't work in Cygwin because of incorrect function definition
  dnl AC_CHECK_LIB(fmod, FSOUND_Close,soundlibs="-lfmod $soundlibs";AC_DEFINE(HAVE_LIBFMOD))

  dnl this is my replacement:
  AC_MSG_CHECKING(for FSOUND_Close in -lfmod)
  OLDLIBS=$LIBS
  LIBS="$LIBS $fmodlibs"
  AC_LINK_IFELSE(
  [
  #include <fmod/fmod.h>

  int main(int argc, char **argv)
  {
      FSOUND_Close();
      return 0;
  }
  ]
  ,soundlibs="$fmodlibs $soundlibs";result="yes";AC_DEFINE([HAVE_LIBFMOD], [], [FMOD lib]), result="no")
  LIBS=$OLDLIBS
  AC_MSG_RESULT($result)

fi


dnl -------------------
dnl Set the c/c++ flags
dnl -------------------
if test "x$ac_cv_header_windows_h" != "xyes" ; then
  if test "x$GXX" != "xyes" ; then
    CFLAGS="$CFLAGS -I/usr/local/include -L/usr/local/lib"
    CXXFLAGS="$CXXFLAGS -I/usr/local/include -L/usr/local/lib"
  fi
else
  CFLAGS="$CFLAGS $MACH -malign-double -I/usr/local/include -L/usr/local/lib"
  CXXFLAGS="$CXXFLAGS $MACH -malign-double -I/usr/local/include -L/usr/local/lib"
fi
CFLAGS="-Wall $CFLAGS"
CXXFLAGS="-Wall $CXXFLAGS"

dnl -------------------
dnl Tell gcc that we want to use ISO 8859-1, and NOT UTF-8
dnl This is because the font texture is in ISO 8859-1
dnl But firs we check whether the compiler accepts these options
dnl -------------------
AC_LANG_PUSH(C)
OLDCFLAGS=$CFLAGS
CFLAGS="-finput-charset=ISO-8859-1 -fexec-charset=ISO-8859-1"
AC_CACHE_CHECK(whether $CC accepts -finput-charset and -fexec-charset, cc_cv_charset_setting,
	       [AC_COMPILE_IFELSE([AC_LANG_PROGRAM()], [cc_cv_charset_setting=yes],
							[cc_cv_charset_setting=no])])
CFLAGS=$OLDCFLAGS
AC_LANG_POP(C)

if test $cc_charset_setting = yes; then
  CFLAGS="-finput-charset=ISO-8859-1 -fexec-charset=ISO-8859-1 $CFLAGS"
  CXXFLAGS="-finput-charset=ISO-8859-1 -fexec-charset=ISO-8859-1 $CXXFLAGS"
fi




dnl -------------------
dnl i18n
dnl -------------------
AC_LANG_PUSH(C)
AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION
AC_LANG_POP(C)

dnl ------------------
dnl The directory locations
dnl ------------------

dnl Set prefix dir explicitly if not already done by user
dnl if (test "$prefix" = "NONE"); then
dnl 	prefix=$ac_default_prefix
dnl fi

dnl The location of the data files:
usdatadir='${datadir}/ultimatestunts/'

dnl The configure-time values of these variables:
dnl sysconfdir_configure_time=`eval eval eval eval echo "${sysconfdir}"`
dnl usdatadir_configure_time=`eval eval eval eval echo "${usdatadir}"`

dnl One directory where ultimatestunts.conf is searched:
dnl AC_DEFINE_UNQUOTED([SYSCONFDIR], ["${sysconfdir_configure_time}"], [Sysconf dir])

dnl Specify sysconfdir directly on commandline (so that it can still be changed at make-time):
CPPFLAGS="-DSYSCONFDIR=\"\\\"\$(sysconfdir)\\\"\" $CPPFLAGS"

AC_SUBST(netlibs)
AC_SUBST(soundlibs)
AC_SUBST(graphlibs)
AC_SUBST(sdllibs)

dnl usdatadir is used as the data file install location
AC_SUBST(usdatadir)

AC_CONFIG_FILES([Makefile intl/Makefile po/Makefile.in data/Makefile shared/Makefile simulation/Makefile graphics/Makefile stuntsserver/Makefile stuntsai/Makefile stunts3dedit/Makefile trackedit/Makefile ultimatestunts/Makefile])
AC_OUTPUT


if (test "$prefix" = "NONE"); then
	prefix=$ac_default_prefix
fi

dnl ------
dnl Output
dnl ------
echo ""
echo "Some important results:"
echo "Network libraries: \"$netlibs\""
echo "Sound libraries: \"$soundlibs\""
echo "Graphics libraries: \"$graphlibs\""
echo "SDL libraries: \"$sdllibs\""
echo "Prefix dir: \"$prefix\""

dnl -------------
dnl Some warnings
dnl -------------
if test "x$soundlibs" = "x"; then
  echo ""
  echo "WARNING: No appropriate sound libraries found."
  echo "  You can still compile Ultimate Stunts, but"
  echo "  sound will not be available"
fi
if test "x$graphlibs" = "x"; then
  echo ""
  echo "WARNING: No appropriate graphics libraries found"
  echo "  You can still compile the non-graphical programs"
fi
if test "x$sdllibs" = "x"; then
  echo ""
  echo "WARNING: SDL library not found"
  echo "  You may still be able to compile the non-graphical programs,"
  echo "  but that is not guaranteed"
fi
if test "x$haveSDLimage" = "xno"; then
  echo ""
  echo "WARNING: SDL_image library not found"
  echo "  It is still perfectly possible to compile Ultimate Stunts,"
  echo "  but it won't be able to load all image formats."
  echo "  This is no problem for the non-graphical programs,"
  echo "  but the graphical programs will not work with all"
  echo "  the default texture files. Only .rgb and .rgba are"
  echo "  supported withoud SDL_image."
fi

echo ""
