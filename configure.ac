AC_INIT([warp],[1.02],[ritchieswann@gmail.com])
AM_INIT_AUTOMAKE(subdir-objects)
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CXX
PKG_CHECK_MODULES(MAGICKWAND,ImageMagick++,[],[AC_MSG_ERROR([Unable to find ImageMagick++])])
PKG_CHECK_MODULES(LIBXML,libxml-2.0,[],[AC_MSG_ERROR([Unable to find libxml])])
LT_INIT
BOOST_REQUIRE
BOOST_FILESYSTEM
BOOST_PROGRAM_OPTIONS
BOOST_SYSTEM
BOOST_THREAD
AC_LANG_PUSH([C++])
AC_CHECK_LIB(morph, warp_image_a_m, [], [AC_MSG_ERROR([Unable to find libmorph])])
AC_CHECK_HEADER(newmat/newmat.h, [], [AC_MSG_ERROR([Unable to find newmat])])
AC_LANG_POP([C++])
AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT
