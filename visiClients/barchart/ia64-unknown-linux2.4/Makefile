#
# $Id: Makefile,v 1.1 2002/02/11 22:01:39 tlmiller Exp $
#

# Define any symbols needed to invoke configuration changes in make.config

USES_TCLTK	= true
USES_X11	= true
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
SYSLIBS		+= -ldl
#SYSLIBS		= -lsocket -lnsl

# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.

include ../../../make.program.tmpl
