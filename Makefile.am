ACLOCAL_AMFLAGS = -I m4 --install
AM_CXXFLAGS     = -I$(top_srcdir)/src/shared \
  -I$(top_srcdir)/src/login \
  -I$(top_srcdir)/src/zone \
  $(SANDBOX_CXXFLAGS) \
  $(BOOST_CPPFLAGS)


noinst_LTLIBRARIES = libshared.la
libshared_la_SOURCES = src/shared/ByteBuffer.cpp \
  src/shared/GalaxyServer.cpp \
  src/shared/Logger.cpp \
  src/shared/LoginHandler.cpp \
  src/shared/PacketTools.cpp \
  src/shared/Player.cpp \
  src/shared/Session.cpp \
  src/shared/SoeMessageFactory.cpp \
  src/shared/UdpSocketListener.cpp
    
libshared_la_LIBADD = $(BOOST_LDFLAGS) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)


noinst_LTLIBRARIES += liblogin.la
liblogin_la_SOURCES = src/login/LoginServer.cpp 
  
liblogin_la_LIBADD = libshared.la \
  $(BOOST_LDFLAGS) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  $(BOOST_PROGRAM_OPTIONS_LIB)


noinst_LTLIBRARIES += libzone.la
libzone_la_SOURCES = src/zone/SpatialHandler.cpp \
  src/zone/ZoneInsertionHandler.cpp \
  src/zone/ZoneServer.cpp 
  
libzone_la_LIBADD = libshared.la \
  $(BOOST_LDFLAGS) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  $(BOOST_PROGRAM_OPTIONS_LIB)


bin_PROGRAMS = cu_sandbox
cu_sandbox_SOURCES = src/main.cpp \
  src/ProgramOptions.cpp
    
cu_sandbox_LDADD = libshared.la \
  liblogin.la \
  libzone.la \
  $(BOOST_LDFLAGS) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  $(BOOST_PROGRAM_OPTIONS_LIB) \
  $(BOOST_REGEX_LIB)

TESTS=cu_sandbox_tests
check_PROGRAMS = $(TESTS)
cu_sandbox_tests_SOURCES = test/main.cpp \
  test/TestByteBuffer.cpp \
  test/TestLoadPacketFromTextFile.cpp

cu_sandbox_tests_CPPFLAGS = $(GTEST_CPPFLAGS)
cu_sandbox_tests_LDADD = libshared.la \
  liblogin.la \
  libzone.la \
  $(BOOST_LDFLAGS) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_THREAD_LIB) \
  $(BOOST_PROGRAM_OPTIONS_LIB) \
  $(BOOST_REGEX_LIB) \
  $(GTEST_LIBS)


doc_DATA = AUTHORS ChangeLog COPYING INSTALL NEWS README

dist-hook:
	-rm -rf 'find $(distdir) -name .svn'