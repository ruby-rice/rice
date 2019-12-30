AC_INIT(rice, 1.1)
AC_CONFIG_SRCDIR(rice)
AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_HEADERS(rice/config.hpp)

AC_LANG(C++)
AC_PROG_CXX
AC_PROG_RANLIB

RICE_CPPFLAGS="$CPPFLAGS"
AC_SUBST(RICE_CPPFLAGS)

RICE_LDFLAGS="$LDFLAGS"
AC_SUBST(RICE_LDFLAGS)

AC_MINGW32
if test x"${MINGW32}"x = xyesx; then
  RICE_USING_MINGW32="true"
else
  RICE_USING_MINGW32="false"
fi

m4_include(ax_cxx_compile_stdcxx.m4)
AX_CXX_COMPILE_STDCXX(14, noext, mandatory)

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

RICE_ROOT=`pwd`
AC_SUBST(RICE_ROOT)

AC_CONFIG_FILES(Makefile rice/Makefile test/Makefile sample/Makefile
                ruby/Makefile ruby/lib/Makefile test/ext/Makefile
                ruby/lib/mkmf-rice.rb rice/detail/ruby_version_code.hpp)
AC_OUTPUT

dnl Work around a bug in older versions of Rubygems
chmod +x install-sh
