#
# $Log: Makefile,v $
# Revision 1.2  2005/12/19 19:44:01  pack
# MRNet replaces xdr for frontend to daemon communication.  New igen  - - -
# MRNet changes
#
# Revision 1.1.1.1  2004/11/05 22:17:51  darnold
# start
#
# Revision 1.1.1.1  2004/10/18 20:35:55  darnold
# Manual Branch for MRNet Integration based on Release_4.1
#
# Revision 1.1  2000/03/31 02:00:31  wylie
# Platform migration
#
# Revision 1.1  1997/10/15 21:21:35  tung
# Linux/X86 Port
#
# Revision 1.1  1996/10/18 23:43:24  mjrg
# Solaris/X86 port
#
#
#

# Define any symbols needed to invoke configuration changes in make.config

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

TO_CORE		= ../../..
USES_TCLTK	= true
USES_X11        = true
NO_IMPLICIT_TEMPLATES = true

include $(TO_CORE)/make.config 

# Now make any necessary architecture specific changes to variables:

SYSLIBS 	+= -ldl
#SYSLIBS 	+= -lsocket -lnsl

# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.

include $(TO_CORE)/make.program.tmpl
