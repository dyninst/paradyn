#
# $Log: Makefile,v $
# Revision 1.2  2005/09/28 17:02:33  bernat
# Makefile rework, Windows fixes.  - - - - - - - - - - - - - - - - - - - -
# Makefile rework; fix AIX paradynd, simplify compilation
#
# Revision 1.1  2003/03/31 17:45:57  buck
# Add new platform rs6000-ibm-aix5.1.
#
# Revision 1.3  2002/02/28 20:34:59  willb
# Removed template instantiation control from make options -- the
# easiest way to get libpdthread building with the rest of paradyn.
#
# For more information about -fexternal-templates,
# -fno-implicit-templates, -frepo, and C++ templates and their
# interaction with the linker, please see the following:
#
# http://gcc.gnu.org/onlinedocs/gcc/C---Dialect-Options.html
# http://gcc.gnu.org/onlinedocs/gcc/Deprecated-Features.html#Deprecated%20Features
# http://gcc.gnu.org/onlinedocs/gcc/Template-Instantiation.html#Template%20Instantiation
#
# Revision 1.2  2002/02/26 22:31:07  willb
# Added NO_IMPLICIT_TEMPLATES before including make.config
#
# Revision 1.1  2002/02/26 19:36:48  willb
# initial revision
#
#
# Define any symbols needed to invoke configuration changes in make.config

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

INSTANTIATE_TEMPLATES = true

TARGET = libpdthread.a

STATIC_LIBRARY = true

include ../../make.config 

# Now make any necessary architecture specific changes to variables:

# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.

include ../../make.library.tmpl
