#
# $Id: Makefile,v 1.1 2005/03/29 22:58:42 igor Exp $
#

# Define any symbols needed to invoke configuration changes in make.config

NO_IMPLICIT_TEMPLATES = true
DO_INLINE_ALL		= true

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

include ../../make.config 

# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

# Now make any necessary architecture specific changes to variables:
#CC = powerpc64-linux-gcc
#CXX = powerpc64-linux-g++
#LD = powerpc64-linux-ld
CFLAGS   += -I/usr/include/libelf -g
CXXFLAGS += $(USEFULWARNINGS) -I/usr/include/libelf -g
SRCS     += ../../common/src/linuxKludges.C \
            ../../common/src/timing-linux.C

OBJS     += linuxKludges.o \
            timing-linux.o

# no support for elf64_XXX() functions
CXXFLAGS += -DUSES_ELF32_ONLY
#CXXFLAGS += -DAGGREGATE_DEBUG
#CXXFLAGS += -DHIST_DEBUG
CFLAGS   += -DUSES_ELF32_ONLY

linuxKludges.o: ../../common/src/linuxKludges.C
	$(CXX) -c $(KFLAGS) ../../common/src/linuxKludges.C

# Include the "standard library template".  This defines all the
# common targets like "clean", "install", etc.

include ../../make.library.tmpl


