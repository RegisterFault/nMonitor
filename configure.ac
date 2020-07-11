AC_INIT([nMonitor], [1.0], [registerfault@gmail.com])
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
LT_PREREQ([2.2])
LT_INIT()
LT_OUTPUT()
AC_PROG_CC
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
                 src/Makefile])
AC_OUTPUT
