#
# $Id: Makefile,v 1.4 2005/02/15 20:00:09 tlmiller Exp $
#
# Define any symbols needed to invoke configuration changes in make.config

# Include standard make configuration stuff that applies to everything
# in the paradyn tree.

INSTANTIATE_TEMPLATES = true
UNCOMMON_LINK = true

include ../../make.config 

# Now make any necessary architecture specific changes to variables:

# Include the module-specific Makefile, which defines everything about
# the module that is common across architectures.

include ../make.module.tmpl

CFLAGS		+= -fPIC
CXXFLAGS	+= -fPIC

# Include the "standard program template".  This defines all the
# common targets like "clean", "install", etc.

include ../../make.library.tmpl

$(TARGET): $(OBJS)
	@$(RM) $@
	@$(MAKE) V_libpdthread
	$(CC) -shared $(LDFLAGS) -o $@ $(VOUCHER).o $(OBJS) $(LIBS) 
