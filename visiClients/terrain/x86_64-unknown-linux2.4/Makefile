#
# $Log: Makefile,v $
# Revision 1.3  2006/10/18 16:07:13  legendre
# AMD64 terrain visi build problem.
#
# Revision 1.2  2006/03/27 17:32:45  igor
# Add uses USES_TCLTK
#
# Revision 1.1  2005/02/24 10:14:27  rchen
# Introduction of the x86_64-unknown-linux2.4 platform.  - - - - - - - - -
# Platform directory structure creation.
#
# Revision 1.1  2000/03/31 02:00:43  wylie
# Platform migration
#
# Revision 1.1  1997/10/15 21:22:46  tung
# Linux/X86 Port
#
# Revision 1.1  1997/05/13 20:55:36  naim
# Adding Terrain to rs6000 and x86 platforms - naim
#
# Revision 1.1  1997/05/12 20:16:37  naim
# Adding "Terrain" visualization to paradyn (commited by naim, done by tung).
#
#
# Define any symbols needed to invoke configuration changes in make.config

USES_X11	= true
USES_TCLTK      = true
TO_CORE		= ../../..
NO_IMPLICIT_TEMPLATES = true

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

include $(TO_CORE)/make.config 

# Now make any necessary architecture specific changes to variables:
# this two are a temporary kludge; barchart should have a "src" dir
VPATH		+= ..
CXXFLAGS	+= $(TCLFRIENDLYWARNINGS)
TFLAGS		+= $(TCLFRIENDLYWARNINGS)
CFLAGS		+= $(TCLFRIENDLYWARNINGS)
SYSLIBS		+= -ldl -lSM -lICE -lXpm
#SYSLIBS		= -lsocket -lnsl

# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.

include ../../../make.program.tmpl
