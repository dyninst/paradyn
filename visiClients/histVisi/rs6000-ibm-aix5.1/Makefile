#
# $Id: Makefile,v 1.3 2007/01/17 22:47:56 giri Exp $
#

# Define any symbols needed to invoke configuration changes in make.config

USES_TCLTK	= true
USES_X11	= true
TO_CORE	    = ../../..
NO_IMPLICIT_TEMPLATES = true

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

include $(TO_CORE)/make.config 

# Now make any necessary architecture specific changes to variables:
# this two are a temporary kludge; barchart should have a "src" dir
CXXFLAGS    += $(TCLFRIENDLYWARNINGS)
TFLAGS	    += $(TCLFRIENDLYWARNINGS)
CFLAGS	    += $(TCLFRIENDLYWARNINGS)

# Override these defaults by setting LDTCLFLAGS in top level make.config.local
# Recommended settig for AIX 4.3.3 w/IBM RPM supplied tcl-8.3.3 is:
# LDTCLFLAGS := -Wl,-brtl,-bbigtoc
LDTCLFLAGS ?= -Xlinker -brtl
LDFLAGS += $(LDTCLFLAGS)

ifdef GCC_2_95
CXXFLAGS	+= -fno-exceptions
TFLAGS		+= -fno-exceptions
endif

# allow users to build tcl/tk libraries as .a's opposed to .so's since
# .so's are difficult to build on AIX.  The .so's automatically pull in the
# pthread library.
LIBS += -lpthread -lsymtabAPI

# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.

include ../../../make.program.tmpl
