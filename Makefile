#
# TopLevel Makefile for the Paradyn (and DynInstAPI) system.
#
# $Id: Makefile,v 1.29 1998/05/21 15:54:27 wylie Exp $
#

# Include the make configuration specification (site configuration options)
include ./make.config
# Include additional local (re-)definitions (if available)
-include ./make.config.local

BUILD_ID = "$(SUITE_NAME) v$(RELEASE_NUM)$(BUILD_MARK)$(BUILD_NUM)"

# "basicComps" is the list of components which need to be built first
# with "make world", since they are used building the rest of the system.
#
# "subSystems" is the list of all other pieces which should be built
# as part of Paradyn.
#
# "dynInstAPI" is the list of additional API components (optional).

basicComps	= util igen thread visi hist
subSystems	= paradyn rtinst rthist paradynd \
		visiClients/tclVisi visiClients/barchart \
		visiClients/tableVisi visiClients/phaseTable \
		visiClients/terrain
dynInstAPI	= util dyninstAPI_RT dyninstAPI dyninstAPI/tests 

# "Paradyn" itself is just the list of all Paradyn components
Paradyn		= $(basicComps) $(subSystems)

# "fullSystem" is the complete list of all Paradyn & DynInstAPI components
fullSystem	= $(basicComps) $(subSystems) $(dynInstAPI)

# Note that the first rule listed ("all") is what gets made by default,
# i.e., if make is given no arguments.  Don't add other targets before all!

all: ready world

# This rule makes most of the normal recursive stuff.  Just about any
# target can be passed down to the lower-level Makefiles by listing it
# as a target here:

clean install depend:
	+@for subsystem in $(fullSystem); do 			\
	    if [ -f $$subsystem/$(PLATFORM)/Makefile ]; then	\
		$(MAKE) -C $$subsystem/$(PLATFORM) $@;		\
	    else						\
		true;						\
	    fi							\
	done

# Check that the main installation directories, etc., are ready for a build,
# creating them if they don't already exist!

ready:
	+@for installdir in $(LIBRARY_DEST) $(PROGRAM_DEST); do \
	    if [ -d $$installdir ]; then			\
		echo "Installation directory $$installdir exists...";	\
	        true;						\
	    else						\
		echo "Creating installation directory $$installdir ...";\
	        mkdir -p $$installdir;				\
	    fi							\
	done
	@echo "Primary compiler for Paradyn build is:"
	@$(CXX) -v

# The "make world" target is set up to build things in the "correct"
# order for a build from scratch.  It builds and installs things in the
# "basicComps" list first, then builds and installs the remaining
# Paradyn "subSystems" (excluding the currently optional dynInstAPI).
# NB: "make world" has also been set up locally to build the dynInstAPI,
# however, this is optional and can be removed if desired.
#
# This make target doesn't go first in the Makefile, though, since we
# really only want to make "install" when it's specifically requested.
# Note that "world" doesn't do a "clean", since it's nice to be able
# to restart a compilation that fails without re-doing a lot of
# unnecessary work.

intro:
	@echo "Building $(BUILD_ID) starting for $(PLATFORM)!"

world: intro Paradyn dynInstAPI
	@echo "Build of $(BUILD_ID) complete for $(PLATFORM)!"

# "make Paradyn" and "make dynInstAPI" are also useful and valid build targets!

Paradyn dynInstAPI basicComps subSystems:
	@echo "Building $@ ..."
	@date
	+for subsystem in $($@); do			\
	    if [ -f $$subsystem/$(PLATFORM)/Makefile ]; then	\
		$(MAKE) -C $$subsystem/$(PLATFORM) all install;	\
	    else						\
		true;						\
	    fi							\
	done
	@echo "Build of $@ complete."
	@date

# This rule passes down the documentation-related make stuff to
# lower-level Makefiles in the individual "docs" directories.

docs install-man:
	+for subsystem in $(fullSystem); do			\
	    if [ -f $$subsystem/docs/Makefile ]; then		\
		$(MAKE) -C $$subsystem/docs $@;			\
	    else						\
		true;						\
	    fi							\
	done


# The "make nightly" target is what should get run automatically every
# night.  It uses "make world" to build things in the right order for
# a build from scratch.  
#
# Note that "nightly" should only be run on the primary build site,
# and does things like building documentation that don't need to be
# built for each different architecture.  Other "non-primary" build
# sites that run each night should just run "make clean world".

nightly:
	$(MAKE) clean
	$(MAKE) world
#	$(MAKE) dynInstAPI
	$(MAKE) docs
	$(MAKE) install-man
	chmod 644 /p/paradyn/man/man?/*
