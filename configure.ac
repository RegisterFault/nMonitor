AC_INIT([nMonitor], [1.0], [registerfault@gmail.com])
AM_INIT_AUTOMAKE([foreign])
LT_PREREQ([2.2])
LT_INIT()
LT_OUTPUT()
AC_PROG_CC
AC_CHECK_HEADER([ncurses.h],
                AC_CHECK_LIB([ncurses],[initscr],,AC_MSG_ERROR([no  ncurses])),
                AC_MSG_ERROR([no ncurses]))
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
