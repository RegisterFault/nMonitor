AC_INIT([nMonitor], [1.0], [registerfault@gmail.com])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE
LT_PREREQ([2.2])
LT_INIT()
LT_OUTPUT()
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CC
PKG_CHECK_MODULES([CHECK], [check],,
                  AC_MSG_NOTICE([no unit testing available]))
AM_PROG_CC_C_O
AC_PROG_LEX([noyywrap])
AM_PROG_LEX
AC_CHECK_FUNCS([bzero memset strstr])
AC_CHECK_HEADERS([fcntl.h])
AC_FUNC_MALLOC()
AC_TYPE_SIZE_T
AC_CHECK_HEADER([ncurses.h],
                AC_CHECK_LIB([ncurses],[initscr],,AC_MSG_ERROR([no  ncurses])),
                AC_MSG_ERROR([no ncurses]))
AC_CHECK_HEADER([math.h],
                AC_CHECK_LIB([m],[pow],,AC_MSG_ERROR([no math])),
                AC_MSG_ERROR([no math]))
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
