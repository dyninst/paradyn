#
# $Id: Makefile,v 1.4 2007/03/22 19:56:27 legendre Exp $
#

# Define any symbols needed to invoke configuration changes in make.config

USES_FLEX = true
NO_IMPLICIT_TEMPLATES = true

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

include ../../make.config 

# Now make any necessary architecture specific changes to variables:

ifdef GCC_2_95
CXXFLAGS	+= -fno-exceptions
TFLAGS		+= -fno-exceptions
endif

# Igen was segfaulting with these added
#CFLAGS += -pthread
#CXXFLAGS += -pthread
#TFLAGS += -pthread
#LDFLAGS += -pthread


# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.
include ../../make.program.tmpl
