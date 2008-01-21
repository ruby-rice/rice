AC_INIT(rice, 1.0)
AC_CONFIG_SRCDIR(rice)
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(rice/config.hpp)

AC_LANG(C++)
AC_PROG_CXX
AC_PROG_RANLIB

RICE_CPPFLAGS="$CPPFLAGS"
AC_SUBST(RICE_CPPFLAGS)

AC_MINGW32
if test x"${MINGW32}"x = xyesx; then
  RICE_USING_MINGW32="true"
else
  RICE_USING_MINGW32="false"
fi

AC_SUBST(RICE_USING_MINGW32)

m4_include(ruby.ac)
RB_INIT_RUBY

m4_include(doxygen.ac)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(rice, Doxyfile, doc)

RICE_SAMPLES="enum map inheritance"
AC_SUBST(RICE_SAMPLES)

AC_CONFIG_FILES(Makefile rice/Makefile test/Makefile sample/Makefile
                ruby/Makefile ruby/lib/Makefile ruby/lib/mkmf-rice.rb)
AC_OUTPUT

