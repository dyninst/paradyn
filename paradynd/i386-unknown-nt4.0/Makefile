#
# $Id: Makefile,v 1.26 2006/07/07 00:01:10 jaw Exp $
#

# Define any symbols needed to invoke configuration changes in make.config

#USES_LIBERTY = true
USES_SHM_SAMPLING = true

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

include ../../nmake.config 

# Now make any necessary architecture specific changes to variables:

SRCS =		../src/pdwinnt.C \
		../src/Object-nt.C \
		../src/init-winnt.C \
		../src/proc-windows.C \
		../src/inst-winnt.C \
		../src/inst-x86.C \
		../src/emit-x86.C \
		../src/arch-x86.C \
		../../rtinst/src/RThwtimer-winnt.c \
		../src/debug.C 

OBJS = 		pdwinnt.obj \
		Object-nt.obj \
		init-winnt.obj \
		proc-windows.obj \
		inst-winnt.obj \
		inst-x86.obj \
		emit-x86.obj \
		arch-x86.obj \
		RThwtimer-winnt.obj \
		debug.obj 

CXXFLAGS 	= $(CXXFLAGS) -DNO_MDL_PRINT -MT

#CXXFLAGS 	= $(CXXFLAGS) -DSAMPLEVALUE_DEBUG
LDFLAGS     = $(LDFLAGS) -NODEFAULTLIB:LIBC.LIB

SYSLIBS	        = $(SYSLIBS) advapi32.lib iphlpapi.lib psapi.lib


# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../nmake.module.tmpl

{../../rtinst/src/}.c.obj:
        $(CC) -c $(CFLAGS) $<

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.

include ../../nmake.program.tmpl


