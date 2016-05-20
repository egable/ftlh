AC_INIT([libftlh], [0.1], [egable@pindropsecurity.com])
AC_CONFIG_SRCDIR([src/ftlh.c])
AM_INIT_AUTOMAKE()
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

AC_ARG_ENABLE(debug_for_gcc_linux_gdb,
AS_HELP_STRING([--enable-debug-for-gcc-linux-gdb],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug_for_gcc_linux_gdb=true ;;
             no)  debug_for_gcc_linux_gdb=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug-for-gcc-linux-gdb]) ;;
esac],
[debug_for_gcc_linux_gdb=false])

AM_CONDITIONAL(GCC_LINUX_DEBUG_GDB, test x"$debug_for_gcc_linux_gdb" = x"true")

AC_CONFIG_FILES([Makefile src/Makefile src/test/Makefile])
AC_OUTPUT
