dnl Process this file with autoconf to produce a configure script.

AC_INIT([Ultimate Stunts], [0.5.3], [cornware-cjp@users.sourceforge.net], [ultimatestunts])
AC_CONFIG_SRCDIR([ultimatestunts/main.cpp])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE

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
odelibs=""

dnl ----------------------------------
dnl check for OpenGL related libraries
dnl this comes from Tuxkart
dnl ----------------------------------
AC_CHECK_HEADER(windows.h)

if test "x$ac_cv_header_windows_h" != "xyes" ; then

  dnl Check for X11

  AC_PATH_XTRA

  x_suffix="$X_LIBS $X_PRE_LIBS -lX11 -lXi -lXext -lXmu $X_EXTRA_LIBS -lm"
  graphlibs=$x_suffix

  dnl Reasonable stuff non-windoze variants ... :-)

  AC_CHECK_LIB(GL, glNewList, graphlibs="-lGL $graphlibs" ,,$graphlibs)
  if test "x$ac_cv_lib_GL_glNewList" = "xno" ; then
    dnl if no GL, check for MesaGL
    AC_CHECK_LIB(MesaGL, glNewList,graphlibs="-lMesaGL $graphlibs",,$graphlibs)
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

  AC_CHECK_LIB(GLU, gluLookAt, graphlibs="-lGLU $graphlibs" ,,$graphlibs)
  if test "x$ac_cv_lib_GLU_gluLookAt" = "xno" ; then
    dnl if no GLU, check for MesaGLU
    AC_CHECK_LIB(MesaGLU, gluLookAt, graphlibs="-lMesaGLU $graphlibs" ,,$graphlibs)
  fi

else
  dnl CygWin under Windoze.

  graphlibs="-lm -lwinmm -lglu32 -lopengl32 -luser32 -lgdi32"

  dnl force a failed check
  AM_CONDITIONAL(ENABLE_XMESA_FX, test "no" = "yes")
fi


dnl -------------
dnl Check for SDL
dnl -------------
AC_LANG_PUSH(C)
AM_PATH_SDL(1.1.0, sdllibs="$SDL_LIBS $sdllibs";AC_DEFINE(HAVE_SDL))
AC_LANG_POP(C)
CFLAGS="$CFLAGS $SDL_CFLAGS"
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"

dnl added "-mconsole" for stdio in windows
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
dnl AC_CHECK_HEADER(windows.h, AC_DEFINE(WIN32))
dnl AC_CHECK_HEADER(joystick.h, AC_DEFINE(JOYSTICK_IS_PRESENT))
dnl AC_CHECK_HEADER(linux/joystick.h, AC_DEFINE(LINUX_JOYSTICK_IS_PRESENT))



dnl TODO: set audio lib with configure command line
dnl ------------------------
dnl Check openAL audio library (currently default)
dnl ------------------------
AC_CHECK_HEADER(AL/al.h, AC_DEFINE(OPENAL_HEADER))
AC_CHECK_LIB(openal, alGetError, soundlibs="-lopenal $soundlibs";AC_DEFINE(HAVE_LIBOPENAL))

if test "x$soundlibs" = "x" ; then

  dnl ------------------------
  dnl Check fmod audio library (if openal not present)
  dnl ------------------------
  AC_CHECK_HEADER(fmod/fmod.h, AC_DEFINE(FMOD_HEADER))

  dnl This doesn't work in Cygwin because of incorrect function definition
  dnl AC_CHECK_LIB(fmod, FSOUND_Close,soundlibs="-lfmod $soundlibs";AC_DEFINE(HAVE_LIBFMOD))

  dnl this is my replacement:
  AC_MSG_CHECKING(for FSOUND_Close in -lfmod)
  OLDLIBS=$LIBS
  LIBS="$LIBS -lfmod"
  AC_LINK_IFELSE(
  [
  #include <fmod/fmod.h>

  int main(int argc, char **argv)
  {
      FSOUND_Close();
      return 0;
  }
  ]
  ,soundlibs="-lfmod $soundlibs";result="yes";AC_DEFINE(HAVE_LIBFMOD), result="no")
  LIBS=$OLDLIBS
  AC_MSG_RESULT($result)

fi


dnl -----------------------
dnl Check for ODE library
dnl -----------------------
AC_CHECK_HEADER(ode/ode.h)
AC_CHECK_LIB(ode, dCloseODE,odelibs="-lode $odelibs")


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

dnl ------------------
dnl The directory locations
dnl ------------------
dnl TODO: set this with some intelligent script
datadir="./data/"
stuntsdir='${prefix}/games/ultimatestunts'

dnl ------
dnl Output
dnl ------
echo ""
echo "Some important results:"
echo "Network libraries: \"$netlibs\""
echo "Sound libraries: \"$soundlibs\""
echo "Graphics libraries: \"$graphlibs\""
echo "SDL libraries: \"$sdllibs\""
echo "ODE libraries: \"$odelibs\""
echo "datadir: \"$datadir\""
echo "stuntsdir: \"$stuntsdir\""

dnl -------------
dnl Some warnings
dnl -------------
if test "x$soundlibs" = "x"; then
  echo ""
  echo "WARNING: No appropriate sound libraries found."
  echo "You can still compile Ultimate Stunts, but"
  echo "sound will not be available"
fi
if test "x$graphlibs" = "x"; then
  echo ""
  echo "WARNING: No appropriate graphics libraries found"
  echo "You can still compile the non-graphical programs"
fi
if test "x$sdllibs" = "x"; then
  echo ""
  echo "WARNING: SDL library not found"
  echo "You may still be able to compile the non-graphical programs,"
  echo "but that is not guaranteed"
fi
if test "x$odelibs" = "x"; then
  echo ""
  echo "WARNING: ODE library not found"
  echo "Ultimate Stunts really depends on ODE, so you will"
  echo "probably only be able to compile the tile editor"
  echo "and the track editor"
fi

echo ""

AC_SUBST(netlibs)
AC_SUBST(soundlibs)
AC_SUBST(graphlibs)
AC_SUBST(sdllibs)
AC_SUBST(odelibs)

AC_SUBST(PACKAGE_VERSION)
AC_SUBST(datadir)
AC_SUBST(stuntsdir)

AC_OUTPUT(ultimatestunts.conf ultimatestunts.sh Makefile data/Makefile shared/Makefile simulation/Makefile graphics/Makefile stuntsserver/Makefile stuntsai/Makefile stunts3dedit/Makefile trackedit/Makefile ultimatestunts/Makefile)
