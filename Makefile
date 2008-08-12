#
# TopLevel Makefile for the Paradyn (and DyninstAPI) system.
#
# $Id: Makefile,v 1.93 2008/08/12 15:50:40 carl Exp $
#

TO_CORE = .
# Include additional local definitions (if available)
-include ./make.config.local
# Include the make configuration specification (site configuration options)
include ./make.config

BUILD_ID = "$(SUITE_NAME) v$(RELEASE_NUM)$(BUILD_MARK)$(BUILD_NUM)"

# "basicComps" is the list of components which need to be built first
# with "make world", since they are used building the rest of the system.
#
# "subSystems" is the list of all other pieces which should be built
# as part of Paradyn.
#
# "DyninstAPI" is the list of additional API components (optional).

basicComps	= igen mrnet pdutil 
ParadynD	= pdutil igen mrnet sharedMem rtinst symtabAPI dyninstAPI dyninstAPI_RT paradynd
ParadynFE	= pdthread paradyn
ParadynVC	= visi \
		visiClients/tclVisi visiClients/barchart \
		visiClients/tableVisi visiClients/phaseTable \
		visiClients/histVisi visiClients/terrain \
		visiClients/termWin

subSystems	= $(ParadynD) $(ParadynFE) $(ParadynVC)
SymtabAPI 	= ready common symtabAPI dynutil
StackwalkerAPI = ready common symtabAPI stackwalk
DyninstAPI	= ready common symtabAPI dyninstAPI_RT dyninstAPI dynutil instructionAPI
InstructionAPI	= ready common instructionAPI dynutil

testsuites = dyninstAPI/tests testsuite 

allSubdirs	= $(subSystems) common dyninstAPI/tests testsuite dynutil instructionAPI stackwalk newtestsuite
allSubdirs_noinstall =

# We're not building the new test suite on all platforms yet
ifeq ($(DONT_BUILD_NEWTESTSUITE),false)
testsuites += newtestsuite
allSubdirs_noinstall += newtestsuite
endif

# "fullSystem" is the list of all Paradyn & DyninstAPI components to build:
# set DONT_BUILD_PARADYN or DONT_BUILD_DYNINST in make.config.local if desired
ifndef DONT_BUILD_PARADYN
fullSystem	+= $(basicComps)
Build_list	+= basicComps
ifndef DONT_BUILD_FE
fullSystem	+= $(ParadynFE)
Build_list	+= ParadynFE
endif
ifndef DONT_BUILD_DAEMON
fullSystem	+= $(ParadynD)
Build_list	+= ParadynD
endif
ifndef DONT_BUILD_VISIS
fullSystem	+= $(ParadynVC)
Build_list	+= ParadynVC
endif
endif

ifndef DONT_BUILD_DYNINST
fullSystem	+= $(DyninstAPI)
Build_list	+= DyninstAPI
endif

# Note that the first rule listed ("all") is what gets made by default,
# i.e., if make is given no arguments.  Don't add other targets before all!

all: ready world

# This rule makes most of the normal recursive stuff.  Just about any
# target can be passed down to the lower-level Makefiles by listing it
# as a target here:

clean depend distclean:
	+@for subsystem in $(fullSystem); do 			\
	    if [ -f $$subsystem/$(PLATFORM)/Makefile ]; then	\
			$(MAKE) -C $$subsystem/$(PLATFORM) $@;		\
	    else						\
			true;						\
	    fi							\
	done

install:	ready world
	+@for subsystem in $(fullSystem); do 			\
		if [ -f $$subsystem/$(PLATFORM)/Makefile ]; then	\
			$(MAKE) -C $$subsystem/$(PLATFORM) install;		\
		elif [ -f $$subsytem/Makefile ]; then 			\
			$(MAKE) -C $$subsystem install; \
	   else	\
			true;						\
	   fi							\
	done	

# Check that the main installation directories, etc., are ready for a build,
# creating them if they don't already exist!  Also touch make.config.local
# if needed.

ready:
	+@for installdir in $(LIBRARY_DEST) $(PROGRAM_DEST) $(INCLUDE_DEST); do \
	    if [ -d $$installdir ]; then			\
		echo "Installation directory $$installdir exists...";	\
	        true;						\
	    else						\
		echo "Creating installation directory $$installdir ...";\
	        mkdir -p $$installdir;				\
	    fi							\
	done

	+@if [ ! -f make.config.local ]; then	\
	    touch make.config.local;		\
	fi

	+@echo "Primary compiler for Paradyn build is:"
	+@if [ `basename $(CXX)` = "xlC" ]; then		\
               echo "xlC"; \
        elif [ `basename $(CXX)` = "insure" ]; then \
		echo "insure"; \
	else \
	  $(CXX) -v; \
     true; \
	fi

# The "make world" target is set up to build things in the "correct"
# order for a build from scratch.  It builds and installs things in the
# "basicComps" list first, then builds and installs the remaining
# Paradyn "subSystems" (excluding the currently optional DyninstAPI).
# NB: "make world" has also been set up locally to build the DyninstAPI,
# however, this is optional and can be removed if desired.
#
# This make target doesn't go first in the Makefile, though, since we
# really only want to make "install" when it's specifically requested.
# Note that "world" doesn't do a "clean", since it's nice to be able
# to restart a compilation that fails without re-doing a lot of
# unnecessary work.

intro:
	@echo "Build of $(BUILD_ID) starting for $(PLATFORM)!"
ifdef DONT_BUILD_PARADYN
	@echo "Build of Paradyn components skipped!"
endif
ifdef DONT_BUILD_FE
	@echo "Build of Paradyn front-end components skipped!"
endif
ifdef DONT_BUILD_DAEMON
	@echo "Build of Paradyn daemon components skipped!"
endif
ifdef DONT_BUILD_VISIS
	@echo "Build of Paradyn visi client components skipped!"
endif
ifdef DONT_BUILD_PD_MT
	@echo "Build of ParadynMT components skipped!"
endif
ifdef DONT_BUILD_DYNINST
	@echo "Build of DyninstAPI components skipped!"
endif

world: intro
	$(MAKE) $(fullSystem)
	@echo "Build of $(BUILD_ID) complete for $(PLATFORM)!"

# "make Paradyn" and "make DyninstAPI" are also useful and valid build targets!

Paradyn ParadynD ParadynFE ParadynVC DyninstAPI SymtabAPI StackwalkerAPI basicComps subSystems testsuites InstructionAPI: 
	$(MAKE) $($@)
	@echo "Build of $@ complete."
	@date

# Low-level directory-targets  (used in the sets defined above)
# Explicit specification of these rules permits better parallelization
# than building subsystems using a for loop

.PHONY: $(allSubdirs) $(allSubdirs_noinstall)

$(allSubdirs): 
	@if [ -f $@/$(PLATFORM)/Makefile ]; then \
		$(MAKE) -C $@/$(PLATFORM) && \
		$(MAKE) -C $@/$(PLATFORM) install; \
	elif [ -f $@/Makefile ]; then \
		$(MAKE) -C $@ && \
		$(MAKE) -C $@ install; \
	else \
		echo $@ has no Makefile; \
		false; \
	fi

$(allSubdirs_noinstall):
	@echo "***allSubdirs_noinstall***"
	+@if [ -f $@/$(PLATFORM)/Makefile ]; then \
		$(MAKE) -C $@/$(PLATFORM); \
	elif [ -f $@/Makefile ]; then \
		$(MAKE) -C $@; \
	else \
		@echo $@ has no Makefile; \
		false; \
	fi

# Generate targets of the form install_<target> for all directories in
# allSubdirs_noinstall
allSubdirs_explicitInstall = $(patsubst %,install_%,$(allSubdirs_noinstall))

$(allSubdirs_explicitInstall): install_%: %
	+@if [ -f $(@:install_%=%)/$(PLATFORM)/Makefile ]; then \
		$(MAKE) -C $(@:install_%=%)/$(PLATFORM) install \
	elif [ -f $(@:install_%=%)/Makefile ]; then \
		$(MAKE) -C $(@:install_%=%) install; \
	else \
		@echo $(@:install_%=%) has no Makefile; \
		false; \
	fi


# dependencies -- keep parallel make from building out of order
symtabAPI igen: common
stackwalk: symtabAPI dynutil
dyninstAPI: symtabAPI instructionAPI
symtabAPI dyninstAPI: dynutil
paradynd:  pdutil dyninstAPI 
paradyn: pdutil pdthread 
pdthread: igen mrnet 
visi:  pdutil
pdutil: igen
visiClients/tclVisi: visi
visiClients/barchart: visi
visiClients/tableVisi: visi
visiClients/phaseTable: visi
visiClients/histVisi: visi
visiClients/terrain: visi
visiClients/termWin: visi mrnet pdthread
dyner codeCoverage dyninstAPI/tests testsuite newtestsuite: dyninstAPI
rtinst: igen dyninstAPI_RT 

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

umd-nightly:
	$(MAKE) clean
	$(MAKE) DyninstAPI

# Used for nightly builds
nightly: DyninstAPI testsuites

#nightly:
#	$(MAKE) clean
#	$(MAKE) world
##	$(MAKE) DyninstAPI
#	$(MAKE) docs
#	$(MAKE) install-man
#	chmod 644 /p/paradyn/man/man?/*
