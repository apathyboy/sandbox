#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([cu_sandbox], [1.0.0], [bugs@openswg.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror])
LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_MACRO_DIR([tools/m4])
AC_CONFIG_SRCDIR([src/sandbox/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
GTEST_LIB_CHECK
AX_BOOST_BASE([1.38])
AX_BOOST_ASIO
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_BOOST_REGEX
CHECK_ZLIB

# Checks for header files.
AC_CHECK_HEADERS([limits.h malloc.h wchar.h])
AC_CXX_HEADER_STDCXX_TR1

if test "$ac_cv_cxx_stdcxx_tr1" = no; then
  AC_MSG_ERROR([C++ TR1 support required to build this application.])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CXX_COMPILE_STDCXX_0X

if test "$ac_cv_cxx_compile_cxx0x_native" = no &&
   test "$ac_cv_cxx_compile_cxx0x_cxx" = no &&
   test "$ac_cv_cxx_compile_cxx0x_gxx" = no; then
  AC_MSG_ERROR([C++0x features required to build this application.])
fi

# Create a container to store custom flags (used by automake files)
SANDBOX_CXXFLAGS=""

# If native C++0x support is not provided then a compiler flag
# is required for g++ to compile the source. Check if the two
# compiler options are supported and add one to the CXXFLAGS,
# prefer standard c++ over gnu
if test "$ac_cv_cxx_compile_cxx0x_native" = no; then
  if test "$ac_cv_cxx_compile_cxx0x_gxx" = yes; then
    SANDBOX_CXXFLAGS="-std=gnu++0x"
  elif test "$ac_cv_cxx_compile_cxx0x_cxx" = yes; then
    SANDBOX_CXXFLAGS="-std=c++0x"
  fi
fi

# Preserve variables to use in automake files
AC_SUBST(SANDBOX_CXXFLAGS)

# Checks for library functions.

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

echo "
  ($PACKAGE_NAME) version $PACKAGE_VERSION
  Prefix.........: $prefix
  C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS $SANDBOX_CXXFLAGS 
                   $BOOST_CPPFLAGS
  Linker.........: $LD $LDFLAGS $BOOST_LDFLAGS 
                   $BOOST_SYSTEM_LIB
                   $BOOST_THREAD_LIB
                   $BOOST_PROGRAM_OPTIONS_LIB
                   $BOOST_REGEX_LIB
"
