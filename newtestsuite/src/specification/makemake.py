import os
import tuples

######################################################################
# Utility functions
######################################################################
#

def uniq(lst):
	return reduce(lambda l, i: ((i not in l) and l.append(i)) or l, lst, [])

info = {}
def read_tuples(tuplefile):
	f = open(tuplefile)
	info['platforms'] = tuples.parse_platforms(f.readline())
	info['languages'] = tuples.parse_languages(f.readline())
	info['compilers'] = tuples.parse_compilers(f.readline())
	info['mutators'] = tuples.parse_mutators(f.readline())
	info['mutatees'] = tuples.parse_mutatees(f.readline())
	info['tests'] = tuples.parse_tests(f.readline())
	info['rungroups'] = tuples.parse_rungroups(f.readline())
# 	info['exception_types'] = tuples.parse_exception_types(f.readline())
	info['exception_types'] = None
# 	info['exceptions'] = tuples.parse_exceptions(f.readline())
	info['exceptions'] = None
	info['objects'] = tuples.parse_object_files(f.readline())
	f.close()

def extension(filename):
	ext_ndx = filename.rfind('.')
	return filename[ext_ndx:]

def replace_extension(name, ext):
	dot_ndx = name.rfind('.')
	return name[:dot_ndx] + ext

# Get the name of the language for the given file.  If more than one
# is possible, I don't really know what to do...  Return a list of all
# the possibilities with compilers on the current platform?
# We should not ever get more than one language for a given extension.  We're
# enforcing that in the tuple generator.
def get_file_lang(filename):
	ext = extension(filename)
	langs = filter(lambda x: ext in x['extensions'], info['languages'])
	# We have a list of all languages which match the extension of the file
	# Filter out all of those languages for which there is no compiler on this
	# platform:
	#  Find the compilers available on this platform
	platform = os.environ.get('PLATFORM')
	comps = filter(lambda x: platform in info['compilers'][x]['platforms'],
				   info['compilers'])
	#  Get a list of all the languages supported by those compilers
	supported_langs = map(lambda x: info['compilers'][x]['languages'],
						  comps)
	supported_langs = reduce(lambda x,y:x+y, supported_langs)
	supported_langs = uniq(supported_langs)
	#  Remove all languages from langs that aren't in supported_langs
	langs = filter(lambda x: x['name'] in supported_langs, langs)
	if len(langs) > 1:
		return langs
	else:
		return langs[0]

def compiler_count(lang):
	plat = os.environ.get('PLATFORM')
	return len(filter(lambda x: lang in info['compilers'][x]['languages']
					        and plat in info['compilers'][x]['platforms']
					  , info['compilers']))

def find_platform(pname):
	plist = filter(lambda p: p['name'] == pname, info['platforms'])
	if len(plist) == 0:
		return None
	else:
		return plist[0]

def find_language(lname):
	llist = filter(lambda l: l['name'] == lname, info['languages'])
	if len(llist) == 0:
		return None
	else:
		return llist[0]

# Returns a string containing the mutatee's compile-time options filename
# component: a string of the form _<compiler>_<ABI>_<optimization>
def mutatee_cto_component(mutatee):
	compiler = info['compilers'][mutatee['compiler']]
	return fullspec_cto_component(compiler,
								  mutatee['abi'],
								  mutatee['optimization'])

# Returns a string containing the mutatee's compile-time options filename
# component for mutatees compiled with an auxilliary compiler
def auxcomp_cto_component(compiler, mutatee):
	return fullspec_cto_component(compiler,
								  mutatee['abi'],
								  mutatee['optimization'])

# Returns a string containing the mutatee's compile-time options filename
# component for a fully specified set of build-time options
def fullspec_cto_component(compiler, abi, optimization):
	retval = "_%s_%s_%s" % (compiler['executable'],
							abi,
							optimization)
	return retval

# Returns a string containing the link-time options component for a fully-
# specified set of link-time options
# NOTE: There are currently no link-time options specified
def fullspec_lto_component():
	return ""

# Returns a string containing the link-time options component for a mutatee's
# filename
# NOTE: There are currently no link-time options specified
def mutatee_lto_component(mutatee):
	return fullspec_lto_component()

# Returns a string containing the link-time options component for a mutatee's
# filename when the mutatee is compiled using an auxilliary compiler
# NOTE: I think this function is pointless
def auxcomp_lto_component(compiler, mutatee):
	return fullspec_lto_component()

# Returns a string containing the build-time options component for a fully-
# specified set of build-time options
def fullspec_bto_component(compiler, abi, optimization):
	return "%s%s" % (fullspec_cto_component(compiler, abi, optimization),
					 fullspec_lto_component())

# Returns a string containing the build-time options component for a mutatee's
# filename
def mutatee_bto_component(mutatee):
	return "%s%s" % (mutatee_cto_component(mutatee),
					 mutatee_lto_component(mutatee))

# Returns a string containing the build-time options component for a mutatee's
# filename, when the mutatee is built using an auxilliary compiler
# NOTE: I don't think this ever happens.
def auxcomp_bto_component(compiler, mutatee):
	return "%s%s" % (auxcomp_cto_component(compiler, mutatee),
					 auxcomp_lto_component(compiler, mutatee))

def mutatee_binary(mutatee):
	# Returns standard name for the solo mutatee binary for this mutatee
	platform = find_platform(os.environ.get('PLATFORM'))
	es = platform['filename_conventions']['executable_suffix']
	return "%s.mutatee_solo%s%s" % (mutatee['name'],
									mutatee_bto_component(mutatee),
									es)

#
######################################################################


######################################################################
# make.mutators.gen
######################################################################
#

def print_mutators_list(out, mutator_dict):
	platform = find_platform(os.environ.get('PLATFORM'))
	LibSuffix = platform['filename_conventions']['library_suffix']
	ObjSuffix = platform['filename_conventions']['object_suffix']

	out.write("######################################################################\n")
	out.write("# A list of all the mutators to be compiled\n")
	out.write("######################################################################\n\n")
	out.write("MUTATORS = ")
	for m in mutator_dict:
		out.write("%s " % (m['name']))
	out.write("\n\n")
	out.write("OBJS_ALL_MUTATORS = ")
	for m in mutator_dict:
		out.write("%s%s " % (m['name'], ObjSuffix))
	out.write("\n\n")

	# Now we'll print out a rule for each mutator..
	for m in mutator_dict:
		# FIXME Don't hardcode $(LIBTESTSUITE)
		out.write("%s%s: " % (m['name'], LibSuffix))
		# Loop through the files listed in this mutator's source list and
		# add object files corresponding to each to the list of dependencies
		for s in m['sources']:
			# Print out the object file for this source file
			out.write("%s%s " % (s[0:-len(extension(s))], ObjSuffix))
		out.write("$(OBJS_FORALL_MUTATORS) $(DEPENDDIR)/%s.dep $(LIBTESTSUITE)\n" % (m['name']))

		# FIXME Make this one better too.  Right now it's copied straight from
		# make.module.tmpl
		out.write("\t$(CXX) -o $@ -shared $(filter %%%s,$^) $(MUTATOR_SO_LDFLAGS) $(LIBDIR) $(LIBS) $(LDFLAGS)\n" % (ObjSuffix))
		out.write("ifndef NO_OPT_FLAG\n")
		out.write("ifdef STRIP_SO\n")
		out.write("\t$(STRIP_SO) $@\n")
		out.write("endif\n")
		out.write("endif\n\n")


def write_make_mutators_gen(filename, tuplefile):
	read_tuples(tuplefile)
	mutator_dict = info['mutators']
	platform = find_platform(os.environ.get('PLATFORM'))
	LibSuffix = platform['filename_conventions']['library_suffix']
	header = """
# This file is automatically generated by the Dyninst testing system.
# For more information, see core/testsuite/src/specification/makemake.py

"""
	rest = """
# Create shared library names from the mutator test names
MUTATORS_SO += $(addsuffix %s,$(MUTATORS))

# And a rule to build all of the libraries
.PHONY: mutators
mutators: $(MUTATORS_SO)

""" % (LibSuffix)

	out = open(filename, "w")
	out.write(header)
	print_mutators_list(out, mutator_dict)
	out.write(rest)
	out.close()

#
######################################################################

######################################################################
# test_info_new.gen.C
######################################################################
#

def write_test_info_new_gen(filename, tuplefile):
	header = """/* This file automatically generated from test specifications.  See
 * specification/spec.pl and specification/makemake.py
 */

#include "test_info_new.h"

// The constructor for TestInfo
TestInfo::TestInfo(unsigned int i, const char *iname, const char *imrname,
                   const char *isoname, const char *ilabel)
  : index(i), name(iname), mutator_name(imrname), soname(isoname),
    label(ilabel), mutator(NULL), disabled(false), enabled(false)
{
}

// Constructor for RunGroup
RunGroup::RunGroup(char *mutatee_name, start_state_t state_init,
                   create_mode_t attach_init, bool ex)
  : mutatee(mutatee_name), state(state_init), useAttach(attach_init),
    customExecution(ex)
{
}
"""
	read_tuples(tuplefile)
	compilers = info['compilers']
	rungroups = info['rungroups']
	out = open(filename, "w")
	out.write(header)
	print_initialize_mutatees(out, rungroups, compilers)
	out.close()

# Return the name of the mutatee executable for this rungroup
def mutatee_filename(rungroup, compilers):
	if rungroup['mutatee'] == 'none':
		retval = ""
	else:
		compiler = compilers[rungroup['compiler']]
		mutatee = rungroup['mutatee']
		bto = fullspec_bto_component(compiler,
									 rungroup['abi'],
									 rungroup['optimization'])
		platform = find_platform(os.environ.get('PLATFORM'))
		es = platform['filename_conventions']['executable_suffix']
		retval = "%s.mutatee_solo%s%s" % (mutatee, bto, es)
	return retval

# Return the name of the mutator for this test
def test_mutator(testname):
	testobj = filter(lambda t: t['name'] == testname, info['tests'])
	if len(testobj) >= 1:
		testobj = testobj[0]
	else:
		# TODO Handle this case better
		testobj = None
	if testobj != None:
		mrname = testobj['mutator']
	else:
		mrname = None
	return mrname


# Builds a text label for a test based on the run group's information
# FIXME This is hardcoded to set grouped to false.  It needs to be fixed to
# support the group mutatee optimization
def build_label(test, mutator, rungroup):
	label = "{test: %s, mutator: %s, grouped: false" % (test, mutator)
	for n in rungroup:
		# We've already dealt with tests and we don't handle groupable yet
		if n not in ['tests', 'groupable']:
			label = label + ", " + n + ": " + rungroup[n]
	label = label + "}"
	return label


def print_initialize_mutatees(out, rungroups, compilers):
	header = """
// Now we insert the test lists into the run groups
void initialize_mutatees(std::vector<RunGroup *> &tests) {
  unsigned int group_count = 0;
  // Keep track of which element each test is, for later use with the resumelog
  unsigned int test_count;
  RunGroup *rg;
"""
	out.write(header)

	platform = find_platform(os.environ.get('PLATFORM'))
	LibSuffix = platform['filename_conventions']['library_suffix']

	# TODO Change these to get the string conversions from a tuple output
	for group in rungroups:
		compiler = info['compilers'][group['compiler']]
		if compiler['presencevar'] != 'true':
			out.write("#ifdef %s\n" % (compiler['presencevar']))
		mutateename = mutatee_filename(group, compilers)
		out.write('  test_count = 0;\n')
		out.write('  rg = new RunGroup("%s", ' % (mutateename))
		if group['start_state'] == 'stopped':
			out.write('STOPPED, ')
		elif group['start_state'] == 'running':
			out.write('RUNNING, ')
		else: # Assuming 'selfstart'
			out.write('SELFSTART, ')
		if group['run_mode'] == 'createProcess':
			out.write('CREATE, ')
		else: # Assuming 'useAttach'
			out.write('USEATTACH, ')
		if group['groupable'] == 'true':
			out.write('false') # !groupable
		else:
			out.write('true') # !groupable
		out.write(');\n')
		for test in group['tests']:
			# Set the tuple string for this test
			# (<test>, <mutatee compiler>, <mutatee optimization>, <create mode>)
			# I need to get the mutator that this test maps to..
			mutator = test_mutator(test)
			ts = build_label(test, mutator, group)
			out.write('  rg->tests.push_back(new TestInfo(test_count++, "%s", "%s", "%s%s", "%s"));\n' % (test, mutator, mutator, LibSuffix, ts))
		out.write('  rg->index = group_count++;\n')
		out.write('  tests.push_back(rg);\n')
		# Close compiler presence #ifdef
		if compiler['presencevar'] != 'true':
			out.write("#endif // defined(%s)\n" % (compiler['presencevar']))
	out.write('}\n')

#
##########

######################################################################
# make.solo_mutatee.gen
######################################################################
#

def collect_mutatee_comps(mutatees):
	comps = []
	for m in mutatees:
		if m['compiler'] != '' and m['compiler'] not in comps:
			comps.append(m['compiler'])
	return comps

# Print makefile variable initializations for all the compilers used for
# makefiles on this platform
def print_mutatee_comp_defs(out):
	out.write("# Define variables for our compilers, if they aren't already defined\n")
	pname = os.environ.get('PLATFORM')
	# TODO Check that we got a string
	comps = filter(lambda c: c != ''
				             and pname in info['compilers'][c]['platforms'],
				   info['compilers'])
	for c in comps:
		if info['compilers'][c]['presencevar'] != 'true':
			out.write("ifdef %s\n" % (info['compilers'][c]['presencevar']))
		out.write('M_%s ?= %s\n' % (info['compilers'][c]['defstring'], info['compilers'][c]['executable']))
		if info['compilers'][c]['presencevar'] != 'true':
			out.write("endif\n")
	out.write('\n')

def print_mutatee_rules(out, mutatees, compiler):
	mut_names = map(lambda x: mutatee_binary(x), mutatees)
	out.write("######################################################################\n")
	out.write("# Mutatees compiled with %s\n" % (mutatees[0]['compiler']))
	out.write("######################################################################\n\n")
	if compiler['presencevar'] != 'true':
		out.write("ifdef %s\n" % (compiler['presencevar']))
		out.write("# We only want to build these targets if the compiler exists\n")
	out.write("SOLO_MUTATEES_%s = " % (compiler['defstring']))
	for m in mut_names:
		out.write("%s " % (m))
	out.write('\n')
	if compiler['presencevar'] != 'true':
		out.write("endif\n")
	out.write("\n")
	out.write("# Now a list of rules for compiling the mutatees with %s\n\n"
			  % (mutatees[0]['compiler']))

	pname = os.environ.get('PLATFORM')
	platform = find_platform(pname)
	ObjSuffix = platform['filename_conventions']['object_suffix']

	# Write rules for building the mutatee executables from the object files
	for (m, n) in zip(mutatees, mut_names):
		out.write("%s: " % (n))
		for f in m['preprocessed_sources']:
			# List all the compiled transformed source files
			# I need to futz with the compiler here to make sure it's correct..
			# FIXME This next line may end up arbitrarily picking the first
			# language from a list of more than one for an extension
			lang = filter(lambda l: extension(f) in l['extensions'],
						  info['languages'])[0]['name']
			if (lang in compiler['languages']):
				cto = mutatee_cto_component(m)
				out.write("%s " % (replace_extension(f, "_solo%s%s"
													 % (cto, ObjSuffix))))
			else: # Preprocessed file compiled with auxilliary compiler
				pname = os.environ.get('PLATFORM')
				# TODO Check that we got a string..
				platform = find_platform(pname)
				# TODO Check that we retrieved a platform object
				aux_comp = platform['auxilliary_compilers'][lang]
				# TODO Verify that we got a compiler
				cto = auxcomp_cto_component(info['compilers'][aux_comp], m)
				out.write("%s " % (replace_extension(f, "_solo%s%s"
													 % (cto, ObjSuffix))))
		# TODO Let's grab the languages used in the preprocessed sources, and
		# save them for later.  We use this to determine which raw sources get
		# compiled with the same options as the preprocessed ones, in the case
		# of a compiler that is used for more than one language (e.g. GCC in
		# tests test1_35 or test_mem)

		# FIXME I'm doing this wrong: the compiler for preprocessed files might
		# not be the compiler that we're testing..
		# Get the compiler..
		maincomp_langs = uniq(info['compilers'][m['compiler']]['languages'])
		pp_langs = uniq(map(lambda x: get_file_lang(x)['name'], m['preprocessed_sources']))
		# So we want to print out a list of object files that go into this
		# mutatee.  For files that can be compiled with m['compiler'], we'll
		# use it, and compile at optimization level m['optimization'].  For
		# other files, we'll just use the appropriate compiler and not worry
		# about optimization levels?
		for f in m['raw_sources']:
			# Figure out whether or not this file can be compiled with
			# m['compiler']
			lang = get_file_lang(f)
			if type(lang) == type([]):
				lang = lang[0] # BUG This may cause unexpected behavior if more
				               # than one language was returned, but more than
							   # one language should never be returned
			if lang['name'] in maincomp_langs:
				# This file is compiled with the main compiler for this mutatee
				cto = mutatee_cto_component(m)
				out.write("%s " % (replace_extension(f, "%s%s"
													 % (cto, ObjSuffix))))
			else:
				# This file is compiled with an auxilliary compiler
				# Find the auxilliary compiler for this language on this
				# platform
				# This assumes that there is only one possible auxilliary
				# compiler for a language ending with the extension of interest
				# on the platform.  This condition is enforced by sanity checks
				# in the specification file.
				aux_comp = platform['auxilliary_compilers'][lang['name']]
				cto = fullspec_cto_component(info['compilers'][aux_comp],
											 m['abi'], 'none')
				out.write("%s " % (replace_extension(f, '%s%s'
													 % (cto, ObjSuffix))))
		# FIXME Check whether the current compiler compiles C files and if not
		# then use the aux compiler for this platform for the mutatee driver
		# object.
		if 'c' in info['compilers'][m['compiler']]['languages']:
			out.write("mutatee_driver_solo_%s_%s%s\n"
					  % (info['compilers'][m['compiler']]['executable'],
						 m['abi'], ObjSuffix))
		else:
			# Get the aux compiler for C on this platform and use it
			aux_c = find_platform(os.environ.get('PLATFORM'))['auxilliary_compilers']['c']
			aux_c = info['compilers'][aux_c]['executable']
			out.write("mutatee_driver_solo_%s_%s%s\n"
					  % (aux_c, m['abi'], ObjSuffix))
		# Print the actions used to link the mutatee executable
		out.write("\t%s -o $@ $(filter %%%s,$^) %s %s "
				  % (platform['linker'] or "$(M_%s)" % compiler['defstring'],
					 ObjSuffix,
					 compiler['flags']['link'],
					 compiler['abiflags'][platform['name']][m['abi']]))
		for l in m['libraries']:
			# Need to include the required libraries on the command line
			# FIXME Use a compiler-specific command-line flag instead of '-l'
			out.write("-l%s " % (l))
		out.write('\n')

		# ***ADD NEW BUILD-TIME ACTIONS HERE***


# Prints all the special object file compile rules for a given compiler
# FIXME This doesn't deal with preprocessed files!
def print_special_object_rules(compiler, out):
	out.write("\n# Exceptional rules for mutatees compiled with %s\n\n"
			  % (compiler))
	objects = filter(lambda o: o['compiler'] == compiler, info['objects'])
	for o in objects:
		# Print a rule to build this object file
		# TODO Convert the main source file name to an object name
		#  * Crap!  I don't know if this is a preprocessed file or not!
		#  * This should be okay soon; I'm removing the proprocessing stage..
		platform = os.environ.get("PLATFORM")
		ofileext = find_platform(platform)['filename_conventions']['object_suffix']
		ofilename = o['object'] + ofileext
		out.write("%s: " % (ofilename))
		for s in o['sources']:
			out.write("../src/%s " % (s))
		for i in o['intermediate_sources']:
			out.write("%s " % (i))
		for d in o['dependencies']:
			out.write("%s " % (d))
		out.write("\n")
		out.write("\t$(M_%s) $(SOLO_MUTATEE_DEFS) " % (info['compilers'][compiler]['defstring']))
		for f in o['flags']:
			out.write("%s " % (f))
		out.write("-o $@ %s " % info['compilers'][compiler]['parameters']['partial_compile'])
		for s in o['sources']:
			out.write("../src/%s " % (s))
		for i in o['intermediate_sources']:
			out.write("%s " % (i))
		out.write("\n")
	out.write("\n")


# Produces a string of compiler flags for compiling mutatee object files with
# the specified build-time options
def object_flag_string(platform, compiler, abi, optimization):
	return "%s %s %s %s %s" % (compiler['flags']['std'],
							   compiler['flags']['mutatee'],
							   compiler['parameters']['partial_compile'],
							   compiler['abiflags'][platform['name']][abi],
							   compiler['optimization'][optimization])


# Prints generic rules for compiling from mutatee boilerplate
def print_patterns(c, out):
	out.write("\n# Generic rules for %s's mutatees and varying optimization levels\n" % (c))

	compiler = info['compilers'][c]
	platform = find_platform(os.environ.get('PLATFORM'))
	ObjSuffix = platform['filename_conventions']['object_suffix']

 	ng_sources = uniq(map(lambda m: m['preprocessed_sources'][0],
						  filter(lambda m: m['name'] != 'none'
								 and m['groupable'] == 'false',
								 info['mutatees'])))
 	g_sources = uniq(map(lambda m: m['preprocessed_sources'][0],
						 filter(lambda m: m['name'] != 'none'
								and m['groupable'] == 'true',
								info['mutatees'])))

	for abi in platform['abis']:
		for o in compiler['optimization']:
			# Rules for compiling source files to .o files
			cto = fullspec_cto_component(compiler, abi, o)

			#FIXME this prints out one rule for every mutatee preprocessed source for EVERY optimization
			#      I don't know whether the previous targets require every combination of source/opt
			#      i.e. printing ALL of them may be superfluous
			for sourcefile in ng_sources:
				ext = extension(sourcefile)
				boilerplate = "solo_mutatee_boilerplate" + ext
				basename = sourcefile[0:-len('_mutatee') - len(ext)]

				out.write("%s_mutatee_solo%s%s: ../src/%s\n"
						% (basename, cto, ObjSuffix, boilerplate))
				out.write("\t$(M_%s) $(SOLO_MUTATEE_DEFS) %s -DTEST_NAME=%s -DGROUPABLE=0 -DMUTATEE_SRC=../src/%s -o $@ $<\n"
						% (compiler['defstring'],
						   object_flag_string(platform, compiler, abi, o),
						   basename, sourcefile))

			for sourcefile in g_sources:
				ext = extension(sourcefile)
				boilerplate = "solo_mutatee_boilerplate" + ext
				basename = sourcefile[0:-len('_mutatee') - len(ext)]

				out.write("%s_mutatee_solo%s%s: ../src/%s\n"
						% (basename, cto, ObjSuffix, boilerplate))
				out.write("\t$(M_%s) $(SOLO_MUTATEE_DEFS) %s -DTEST_NAME=%s -DGROUPABLE=1 -DMUTATEE_SRC=../src/%s -o $@ $<\n"
						% (compiler['defstring'],
						   object_flag_string(platform, compiler, abi, o),
						   basename, sourcefile))

			for l in compiler['languages']:
				lang = find_language(l) # Get language dictionary from name
				for e in lang['extensions']:
					# FIXME This generates spurious lines for compilers that
					# aren't used for this part of the mutatee build system
					# like .s and .S files for gcc and Fortran files.
					out.write("%%%s%s: ../src/%%%s\n"
							  % (cto, ObjSuffix, e))
					out.write("\t$(M_%s) $(SOLO_MUTATEE_DEFS) %s -o $@ $<\n"
							  % (compiler['defstring'],
								 object_flag_string(platform, compiler,
													abi, o)))
	out.write("\n")


# Prints pattern rules for this platform's auxilliary compilers
def print_aux_patterns(out, platform, comps):
	# Pattern rules for auxilliary compilers supported on this platform
	out.write("\n# Generic rules for this platform's auxilliary compilers\n\n")
	aux_comps = platform['auxilliary_compilers']
	for ac_lang in aux_comps:
		compname = aux_comps[ac_lang]
		comp = info['compilers'][compname]
		# Print pattern rule(s) for this compiler
		# ac should be a map from a language to a compiler..
		lang = filter(lambda l: l['name'] == ac_lang, info['languages'])[0]
		for ext in lang['extensions']:
			for abi in platform['abis']:
				for o in comp['optimization']:
					cto = fullspec_cto_component(comp, abi, o)
					out.write("%%%s%s: ../src/%%%s\n"
							  % (cto,
								 platform['filename_conventions']['object_suffix'],
								 ext))
					# FIXME Make this read the parameter flags from the
					# compiler tuple (output file parameter flag)
					out.write("\t$(M_%s) %s -o $@ $<\n\n"
							  % (comp['defstring'],
								 object_flag_string(platform, comp,
													abi, o)))

	# Also want to generate rules for any non-standard objects built with
	# auxilliary compilers..
	# Need to find the difference aux_comps \ comps..
	for l in aux_comps:
		c = platform['auxilliary_compilers'][l]
		if c not in comps:
			print_special_object_rules(c, out)

# Prints the footer for make.solo_mutatee.gen
def print_make_solo_mutatee_gen_footer(out, comps, platform):
	compilers = info['compilers']
	out.write("# And a rule to build all of the mutatees\n")
	out.write(".PHONY: solo_mutatees\n")
	out.write("solo_mutatees: ")
	for c in comps:
		out.write("$(SOLO_MUTATEES_%s) " % (compilers[c]['defstring']))
	out.write("\n\n")
	out.write(".PHONY: clean_solo_mutatees\n")
	out.write("clean_solo_mutatees:\n")
	for c in comps:
		out.write("\t-$(RM) $(SOLO_MUTATEES_%s)\n"
				  % (compilers[c]['defstring']))
	# Get a list of optimization levels we're using and delete the mutatee
	# objects for each of them individually
	objExt = platform['filename_conventions']['object_suffix']
	# FIXME Get the actual list of optimization levels we support
	for o in ['none', 'low', 'high', 'max']:
		out.write("\t-$(RM) *_%s%s\n" % (o, objExt))
	out.write("\n\n")
	out.write("SOLO_MUTATEES =")
	for c in comps:
		out.write(" $(SOLO_MUTATEES_%s)" % (compilers[c]['defstring']))
	out.write("\n\n")

	out.write("### COMPILER_CONTROL_DEFS is used to determine which compilers are present\n")
	out.write("### when compiling the list of mutatees to test against\n")
	out.write("COMPILER_CONTROL_DEFS =\n")
	for c in comps:
		if info['compilers'][c]['presencevar'] != 'true':
			out.write("ifdef %s # Is %s present?\n" % (info['compilers'][c]['presencevar'], c))
			out.write("COMPILER_CONTROL_DEFS += -D%s\n" % (info['compilers'][c]['presencevar']))
			out.write("endif\n")
	out.write('\n')


# Main function for generating make.solo_mutatee.gen
def write_make_solo_mutatee_gen(filename, tuplefile):
	read_tuples(tuplefile)
	compilers = info['compilers']
	mutatees = info['mutatees']
	out = open(filename, "w")
	print_mutatee_comp_defs(out)
	comps = collect_mutatee_comps(mutatees)
	pname = os.environ.get('PLATFORM')
	platform = find_platform(pname)
	ObjSuffix = platform['filename_conventions']['object_suffix']
	for c in comps:
		# Generate a block of rules for mutatees produced by each compiler
		muts = filter(lambda x: x['compiler'] == c, mutatees)
		print_mutatee_rules(out, muts, compilers[c])
		# Print rules for exceptional object files
		print_special_object_rules(c, out)
		print_patterns(c, out)
		out.write("# Rules for building the driver and utility objects\n")
		# TODO Replace this code generation with language neutral code
		# generation
		if 'c' in info['compilers'][c]['languages']:
			for abi in platform['abis']:
				out.write("mutatee_driver_solo_%s_%s%s: ../src/mutatee_driver.c\n" % (info['compilers'][c]['executable'], abi, ObjSuffix))
				out.write("\t$(M_%s) %s %s %s -o $@ -c $<\n"
						  % (compilers[c]['defstring'],
							 compilers[c]['flags']['std'],
							 compilers[c]['flags']['mutatee'],
							 compilers[c]['abiflags'][platform['name']][abi]))
				out.write("mutatee_util_%s_%s%s: ../src/mutatee_util.c\n"
						  % (info['compilers'][c]['executable'],
							 abi, ObjSuffix))
				out.write("\t$(M_%s) %s %s %s -o $@ -c $<\n\n"
						  % (compilers[c]['defstring'],
							 compilers[c]['flags']['std'],
							 compilers[c]['flags']['mutatee'],
							 compilers[c]['abiflags'][platform['name']][abi]))
		else:
			out.write("# (Skipped: driver and utility objects cannot be compiled with this compiler\n")

	# Print pattern rules for this platform's auxilliary compilers
	print_aux_patterns(out, platform, comps)

	# Print footer (list of targets, clean rules, compiler presence #defines)
	print_make_solo_mutatee_gen_footer(out, comps, platform)

	out.close()
#
##########

# --------------------------------------------------------
# --------------------------------------------------------
# BEGIN NT SPECIFIC PROCEDURES
# --------------------------------------------------------
# --------------------------------------------------------

# Return the name of the mutatee executable for this rungroup
# (Based on compiler name, NOT compiler executable name
def mutatee_filename_nt(rungroup, compilers):
	if rungroup['mutatee'] == 'none':
		retval = ""
	else:
		compiler = compilers[rungroup['compiler']]
		mutatee = rungroup['mutatee']
		bto = fullspec_cto_component_nt(compiler,
									 rungroup['abi'],
									 rungroup['optimization'])
		platform = find_platform(os.environ.get('PLATFORM'))
		es = platform['filename_conventions']['executable_suffix']
		retval = "%s.mutatee_solo%s%s" % (mutatee, bto, es)
	return retval


# These functions are duplicated because the compiler NAME, not executable name,
# needs to be used for windows, since the exec. name is the same for both c and c++
def mutatee_cto_component_nt(mutatee):
	compiler = info['compilers'][mutatee['compiler']]
	return fullspec_cto_component_nt(compiler,
								  mutatee['abi'],
								  mutatee['optimization'])

def auxcomp_cto_component_nt(compiler, mutatee):
	return fullspec_cto_component_nt(compiler,
								  mutatee['abi'],
								  mutatee['optimization'])

def fullspec_cto_component_nt(compiler, abi, optimization):
	def reverse_lookup(dict, value):
		for key in dict:
			if dict[key] == value:
				return key
		raise ValueError

	retval = "_%s_%s_%s" % (reverse_lookup(info['compilers'], compiler),
							abi,
							optimization)
	return retval

def mutatee_binary_nt(mutatee):
	# Returns standard name for the solo mutatee binary for this mutatee
	# (for windows)
	platform = find_platform(os.environ.get('PLATFORM'))
	es = platform['filename_conventions']['executable_suffix']
	return "%s.mutatee_solo%s%s" % (mutatee['name'],
									mutatee_cto_component_nt(mutatee),
									es)

#TODO this function has literally one line differnet from the non _nt version.
#     merge?
def write_test_info_new_gen_nt(filename, tuplefile):
	header = """/* This file automatically generated from test specifications.  See
 * specification/spec.pl and specification/makemake.py
 */

#include "test_info_new.h"

// The constructor for TestInfo
TestInfo::TestInfo(unsigned int i, const char *iname, const char *imrname,
                   const char *isoname, const char *ilabel)
  : index(i), name(iname), mutator_name(imrname), soname(isoname),
    label(ilabel), mutator(NULL), disabled(false), enabled(false)
{
}

// Constructor for RunGroup, with an initial test specified
RunGroup::RunGroup(char *mutatee_name, start_state_t state_init,
                   create_mode_t attach_init, bool ex, TestInfo *test_init)
  : mutatee(mutatee_name), state(state_init), useAttach(attach_init),
    customExecution(ex)
{
  tests.push_back(test_init);
}

// Constructor for RunGroup with no initial test specified
RunGroup::RunGroup(char *mutatee_name, start_state_t state_init,
                   create_mode_t attach_init, bool ex)
  : mutatee(mutatee_name), state(state_init), useAttach(attach_init),
    customExecution(ex)
{
}
"""
	read_tuples(tuplefile)
	compilers = info['compilers']
	rungroups = info['rungroups']
	out = open(filename, "w")
	out.write(header)
	print_initialize_mutatees_nt(out, rungroups, compilers)
	out.close()

def print_initialize_mutatees_nt(out, rungroups, compilers):
# in visual studio 2003, exceeding 1920 'exception states' causes the compiler to crash.
# from what I can tell, each instantation of an object creates an 'exception case,' so
# the workaround is to instantiate dynamically
	header = """
// Now we insert the test lists into the run groups
void initialize_mutatees(std::vector<RunGroup *> &tests) {
  unsigned int group_count = 0;
  // Keep track of which element each test is, for later use with the resumelog
  unsigned int test_count;
  RunGroup *rg;

"""
	out.write(header)

	platform = find_platform(os.environ.get('PLATFORM'))
	LibSuffix = platform['filename_conventions']['library_suffix']

	rungroup_params = []
	test_params = []
	# TODO Change these to get the string conversions from a tuple output
	for group in rungroups:
		compiler = info['compilers'][group['compiler']]
		if compiler['presencevar'] == 'true':
			presencevar = 'true'
		else:
			presencevar = 'false'
#			out.write("#ifdef %s\n" % (compiler['presencevar']))
		mutatee_name = mutatee_filename_nt(group, compilers)
#		out.write('  test_count = 0;\n')
#		out.write('  rg = new RunGroup("%s", ' % (mutateename))
		if group['start_state'] == 'stopped':
#			out.write('STOPPED, ')
			state_init = 'STOPPED'
		elif group['start_state'] == 'running':
#			out.write('RUNNING, ')
			state_init = 'RUNNING'
		else: # Assuming 'selfstart'
#			out.write('SELFSTART, ')
			state_init = 'SELFSTART'
		if group['run_mode'] == 'createProcess':
#			out.write('CREATE, ')
			attach_init = 'CREATE'
		else: # Assuming 'useAttach'
#			out.write('USEATTACH, ')
			attach_init = 'USEATTACH'
		if group['groupable'] == 'true':
#			out.write('false') # !groupable
			ex = 'false'
		else:
#			out.write('true') # !groupable
			ex = 'true'
#		out.write(');\n')
		rungroup_params.append({'presencevar': presencevar, 'mutatee_name': mutatee_name, 'state_init': state_init, 'attach_init': attach_init, 'ex': ex})

		for test in group['tests']:
			# Set the tuple string for this test
			# (<test>, <mutatee compiler>, <mutatee optimization>, <create mode>)
			# I need to get the mutator that this test maps to..
			mutator = test_mutator(test)
			ts = build_label(test, mutator, group)
#			out.write('  rg->tests.push_back(new TestInfo(test_count++, "%s", "%s", "%s%s", "%s"));\n' % (test, mutator, mutator, LibSuffix, ts))
			test_params.append({'test': test, 'mutator': mutator, 'LibSuffix': LibSuffix, 'ts': ts, 'endrungroup': 'false'})
		test_params[-1]['endrungroup'] = 'true'
#		out.write('  rg->index = group_count++;\n')
#		out.write('  tests.push_back(rg);\n')
		# Close compiler presence #ifdef
#		if compiler['presencevar'] != 'true':
#			out.write("#endif // defined(%s)\n" % (compiler['presencevar']))

	body = """struct {
    char * mutatee_name;
    start_state_t state_init;
    create_mode_t attach_init;
    bool ex;
    bool presencevar;
  } rungroup_params[] = {"""
	out.write(body)

	
	out.write(' {"%s", %s, %s, %s, %s}' % (rungroup_params[0]['mutatee_name'], rungroup_params[0]['state_init'], rungroup_params[0]['attach_init'], rungroup_params[0]['ex'], rungroup_params[0]['presencevar']))
	for i in range(1, len(rungroup_params)):
		out.write(', {"%s", %s, %s, %s, %s}' % (rungroup_params[i]['mutatee_name'], rungroup_params[i]['state_init'], rungroup_params[i]['attach_init'], rungroup_params[i]['ex'], rungroup_params[i]['presencevar']))

	body = """ };

  struct {
    bool endrungroup;
    const char * iname;
    const char * mrname;
    const char * isoname;
    const char * ilabel;
  } test_params[] = {"""

	out.write(body)

	out.write(' {%s, "%s", "%s", "%s%s", "%s"}' % (test_params[0]['endrungroup'], test_params[0]['test'], test_params[0]['mutator'], test_params[0]['mutator'], test_params[0]['LibSuffix'], test_params[0]['ts']))
	for i in range(1, len(test_params)):
		out.write(', {%s, "%s", "%s", "%s%s", "%s"}' % (test_params[i]['endrungroup'], test_params[i]['test'], test_params[i]['mutator'], test_params[i]['mutator'], test_params[i]['LibSuffix'], test_params[i]['ts']))

#TODO presencevar
	body = """ };

  int tp_index = 0;
  for (int i = 0; i < %d; i++) {
    test_count = 0;
    rg = new RunGroup(rungroup_params[i].mutatee_name, rungroup_params[i].state_init, rungroup_params[i].attach_init, rungroup_params[i].ex);
    
    do {
      rg->tests.push_back(new TestInfo(test_count++, test_params[tp_index].iname, test_params[tp_index].mrname, test_params[tp_index].isoname, test_params[tp_index].ilabel));
      tp_index++;
    } while (tp_index < %d && test_params[tp_index].endrungroup == false);

    rg->index = group_count++;
    tests.push_back(rg);
  }
}"""

	out.write(body % (len(rungroup_params), len(rungroup_params)))

# ----------------------------
# nmake.mutators.gen (windows)
# ----------------------------
def print_mutators_list_nt(out, mutator_dict):
	platform = find_platform(os.environ.get('PLATFORM'))
	LibSuffix = platform['filename_conventions']['library_suffix']
	ObjSuffix = platform['filename_conventions']['object_suffix']

	out.write("######################################################################\n")
	out.write("# A list of all the mutators to be compiled\n")
	out.write("######################################################################\n\n")
	out.write("MUTATORS = ")
	for m in mutator_dict:
		out.write("%s " % (m['name']))
	out.write("\n\n")
	out.write("OBJS_ALL_MUTATORS = ")
	for m in mutator_dict:
		out.write("%s%s " % (m['name'], ObjSuffix))
	out.write("\n\n")

	# Now we'll print out a rule for each mutator..
	for m in mutator_dict:
		# FIXME Don't hardcode $(LIBTESTSUITE)
		out.write("%s%s: " % (m['name'], LibSuffix))
		# Loop through the files listed in this mutator's source list and
		# add object files corresponding to each to the list of dependencies
		srcs = ''
		for s in m['sources']:
			# Print out the object file for this source file
			srcs += '%s%s ' % (s[0:-len(extension(s))], ObjSuffix)
#		out.write("$(OBJS_FORALL_MUTATORS) $(DEPENDDIR)/%s.dep $(LIBTESTSUITE)\n" % (m['name']))
		# TODO proper dependencies
		out.write("%s$(LIBTESTSUITE)\n" % (srcs))

		# FIXME Make this one better too.  Right now it's copied straight from
		# make.module.tmpl
		out.write("\t$(LINK) $(LDFLAGS) -DLL -out:$@ %s$(MUTATOR_LIBS)\n\n" % (srcs))
#		out.write("ifndef NO_OPT_FLAG\n")
#		out.write("ifdef STRIP_SO\n")
#		out.write("\t$(STRIP_SO) $@\n")
#		out.write("endif\n")
#		out.write("endif\n\n")

def write_make_mutators_gen_nt(filename, tuplefile):
	read_tuples(tuplefile)
	mutator_dict = info['mutators']
	platform = find_platform(os.environ.get('PLATFORM'))
	LibSuffix = platform['filename_conventions']['library_suffix']
	header = """
# This file is automatically generated by the Dyninst testing system.
# For more information, see core/testsuite/src/specification/makemake.py

"""
	interm = """
# Create shared library names from the mutator test names
MUTATORS_SO = """
	for m in mutator_dict:
		interm += "%s%s " % (m['name'], LibSuffix)

	rest = """

$(OBJS_ALL_MUTATORS): ../src/$*.C
	$(CXX) $(CXXFLAGS_NORM) -Dnative_cc=VC.exe -Dnative_cxx=_VCC++.exe -I../src/ -c ../src/$**

# And a rule to build all of the libraries
.PHONY: mutators
mutators: $(MUTATORS_SO)

"""

	out = open(filename, "w")
	out.write(header)
	print_mutators_list_nt(out, mutator_dict)
	out.write(interm)
	out.write(rest)
	out.close()

# --------------------------------
# --------------------------------
# nmake.solo_mutatee.gen (windows)
# --------------------------------
# --------------------------------

# Print makefile variable initializations for all the compilers used for
# makefiles on this platform
def print_mutatee_comp_defs_nt(out):
	out.write("# Define variables for our compilers\n")
	pname = os.environ.get('PLATFORM')
	# TODO Check that we got a string
	comps = filter(lambda c: c != ''
				             and pname in info['compilers'][c]['platforms'],
				   info['compilers'])
	for c in comps:
		if info['compilers'][c]['presencevar'] != 'true':
			out.write("!ifdef %s\n" % (info['compilers'][c]['presencevar']))
		out.write('M_%s = %s\n' % (info['compilers'][c]['defstring'], info['compilers'][c]['executable']))
		if info['compilers'][c]['presencevar'] != 'true':
			out.write("!endif\n")
	out.write('\n')

def print_mutatee_rules_nt(out, mutatees, compiler, unique_target_dict):
	mut_names = map(lambda x: mutatee_binary_nt(x), mutatees)
	out.write("######################################################################\n")
	out.write("# Mutatees compiled with %s\n" % (mutatees[0]['compiler']))
	out.write("######################################################################\n\n")
	if compiler['presencevar'] != 'true':
		out.write("!ifdef %s\n" % (compiler['presencevar']))
		out.write("# We only want to build these targets if the compiler exists\n")
	out.write("SOLO_MUTATEES_%s = " % (compiler['defstring']))
	for m in mut_names:
		out.write("%s " % (m))
	out.write('\n')
	if compiler['presencevar'] != 'true':
		out.write("!endif\n")
	out.write("\n")
	out.write("# Now a list of rules for compiling the mutatees with %s\n\n"
			  % (mutatees[0]['compiler']))

	pname = os.environ.get('PLATFORM')
	platform = find_platform(pname)
	ObjSuffix = platform['filename_conventions']['object_suffix']

	# this variable is used to keep track of dependency targets. this must
	# be done for windows because nmake doesn't have % wildcard support.
	# note: this excludes a mutattee's ['preprocessed_sources'][0], since
	# this is a special case accounted for in print_patterns_nt
	dependency_sources = {}
	# Write rules for building the mutatee executables from the object files
	for (m, n) in zip(mutatees, mut_names):
		out.write("%s: " % (n))
		for f in m['preprocessed_sources']:
			# List all the compiled transformed source files
			# I need to futz with the compiler here to make sure it's correct..
			# FIXME This next line may end up arbitrarily picking the first
			# language from a list of more than one for an extension
			lang = filter(lambda l: extension(f) in l['extensions'],
						  info['languages'])[0]['name']
			if (lang in compiler['languages']):
				cto = mutatee_cto_component_nt(m)
				pps_name = replace_extension(f, "_solo%s%s"
													% (cto, ObjSuffix))
				out.write("%s " % (pps_name))
				# only add unique elements to the sources dictionary,
				# and don't add the first one, since it's rule is printed
				# in print_patterns_nt
				if f != m['preprocessed_sources'][0]:
					# only insert this if it hasn't been accounted for in a
					# previous invocation of this procedure
					try:
						unique_target_dict[pps_name]
					except KeyError:
						# only insert it if it's not already in the list
						try:
							dependency_sources[pps_name]
						except KeyError:
							dependency_sources[pps_name] = {'src': f, 'abi': m['abi'],
									'optimization': m['optimization'], 'suffix': ObjSuffix, 'compiler': compiler}
			else: # Preprocessed file compiled with auxilliary compiler
				pname = os.environ.get('PLATFORM')
				# TODO Check that we got a string..
				platform = find_platform(pname)
				# TODO Check that we retrieved a platform object
				aux_comp = platform['auxilliary_compilers'][lang]
				# TODO Verify that we got a compiler
				cto = auxcomp_cto_component_nt(info['compilers'][aux_comp], m)
				out.write("%s " % (replace_extension(f, "_solo%s%s"
													 % (cto, ObjSuffix))))
				# TODO
				# the same try/except block can be duplicated as above, but
				# I don't think windows ever has preprocessed files compiled
				# with auxilliary compiler. if it does though, the script
				# will die here
				makeshift_die
		# TODO Let's grab the languages used in the preprocessed sources, and
		# save them for later.  We use this to determine which raw sources get
		# compiled with the same options as the preprocessed ones, in the case
		# of a compiler that is used for more than one language (e.g. GCC in
		# tests test1_35 or test_mem)

		# FIXME I'm doing this wrong: the compiler for preprocessed files might
		# not be the compiler that we're testing..
		# Get the compiler..
		maincomp_langs = uniq(info['compilers'][m['compiler']]['languages'])
		pp_langs = uniq(map(lambda x: get_file_lang(x)['name'], m['preprocessed_sources']))
		# So we want to print out a list of object files that go into this
		# mutatee.  For files that can be compiled with m['compiler'], we'll
		# use it, and compile at optimization level m['optimization'].  For
		# other files, we'll just use the appropriate compiler and not worry
		# about optimization levels?
		for f in m['raw_sources']:
			# Figure out whether or not this file can be compiled with
			# m['compiler']
			lang = get_file_lang(f)
			if type(lang) == type([]):
				lang = lang[0] # BUG This may cause unexpected behavior if more
				               # than one language was returned, but more than
							   # one language should never be returned
			if lang['name'] in maincomp_langs:
				# This file is compiled with the main compiler for this mutatee
				cto = mutatee_cto_component_nt(m)
				comp = compiler
				opt = m['optimization']
				rs_name = replace_extension(f, "%s%s"
													% (cto, ObjSuffix))
			else:
				# This file is compiled with an auxilliary compiler
				# Find the auxilliary compiler for this language on this
				# platform
				# This assumes that there is only one possible auxilliary
				# compiler for a language ending with the extension of interest
				# on the platform.  This condition is enforced by sanity checks
				# in the specification file.
				aux_comp = platform['auxilliary_compilers'][lang['name']]
				comp = info['compilers'][aux_comp]
				opt = 'none'
				cto = fullspec_cto_component_nt(info['compilers'][aux_comp],
											 m['abi'], 'none')
				rs_name = replace_extension(f, '%s%s'
													% (cto, ObjSuffix))
			out.write("%s " % (rs_name))

			# only insert if it hasn't been accounted for in a previous
			# invocation of this function
			try:
				unique_target_dict[rs_name]
			except KeyError:
				try:
					dependency_sources[rs_name]
				except KeyError:
					dependency_sources[rs_name] = {'src': f, 'abi': m['abi'],
							'optimization': opt, 'suffix': ObjSuffix, 'compiler': comp}
		# FIXME Check whether the current compiler compiles C files and if not
		# then use the aux compiler for this platform for the mutatee driver
		# object.
		if 'c' in info['compilers'][m['compiler']]['languages']:
			out.write("mutatee_driver_solo_%s_%s%s\n"
					  % (m['compiler'],
						 m['abi'], ObjSuffix))
		else:
			# Get the aux compiler for C on this platform and use it
			aux_c = find_platform(os.environ.get('PLATFORM'))['auxilliary_compilers']['c']
			aux_c = info['compilers'][aux_c]['executable']
			out.write("mutatee_driver_solo_%s_%s%s\n"
					  % (aux_c, m['abi'], ObjSuffix))
		# Print the actions used to link the mutatee executable
		out.write("\t%s %s -out:$@ $** %s "
				  % (platform['linker'] or "$(M_%s)" % compiler['defstring'],
					 compiler['flags']['link'],
					 compiler['abiflags'][platform['name']][m['abi']]))
		# TODO: libraries
		#for l in m['libraries']:
		# Need to include the required libraries on the command line
		# FIXME Use a compiler-specific command-line flag instead of '-l'
		#	out.write("-l%s " % (l))
		out.write('\n')

		# ***ADD NEW BUILD-TIME ACTIONS HERE***

	# now write the individual preprocessed source targets
	for object, options in dependency_sources.iteritems():
		ObjSuffix = options['suffix']
		src = options['src']
		abi = options['abi']
		o = options['optimization']
		comp = options['compiler']

		out.write("%s: ../src/%s\n" % (object, src))
		# FIXME -Dsnprintf=_snprintf is needed for c files that contain snprintf,
		#       but not for .asm files. the masm compiler accepts the -D parameter,
		#       but it's unnecessary
		out.write("\t$(M_%s) %s -Dsnprintf=_snprintf -Fo$@ $**\n"
					% (comp['defstring'], object_flag_string(platform, comp, abi, o)))

		# now add the object to the unique target list. this prevents the same
		# target from being specified in a different invocation of this function
		unique_target_dict[object] = 1


# Prints generic rules for compiling from mutatee boilerplate
def print_patterns_nt(c, out):
	out.write("\n# Generic rules for %s's mutatees and varying optimization levels\n" % (c))

	compiler = info['compilers'][c]
	platform = find_platform(os.environ.get('PLATFORM'))
	ObjSuffix = platform['filename_conventions']['object_suffix']

 	ng_sources = uniq(map(lambda m: m['preprocessed_sources'][0],
						  filter(lambda m: m['name'] != 'none'
								 and m['groupable'] == 'false',
								 info['mutatees'])))
 	g_sources = uniq(map(lambda m: m['preprocessed_sources'][0],
						 filter(lambda m: m['name'] != 'none'
								and m['groupable'] == 'true',
								info['mutatees'])))

	for abi in platform['abis']:
		for o in compiler['optimization']:
			# Rules for compiling source files to .o files
			cto = fullspec_cto_component_nt(compiler, abi, o)

			#FIXME this prints out one rule for every mutatee preprocessed source for EVERY optimization
			#      I don't know whether the previous targets require every combination of source/opt
			#      i.e. printing ALL of them may be superfluous
			for sourcefile in ng_sources:
				ext = extension(sourcefile)
				boilerplate = "solo_mutatee_boilerplate" + ext
				basename = sourcefile[0:-len('_mutatee') - len(ext)]

				out.write("%s_mutatee_solo%s%s: ../src/%s\n"
						% (basename, cto, ObjSuffix, boilerplate))
				out.write("\t$(M_%s) %s -DTEST_NAME=%s -DGROUPABLE=0 -DMUTATEE_SRC=../src/%s -Fo$@ $**\n"
						% (compiler['defstring'],
						   object_flag_string(platform, compiler, abi, o),
						   basename, sourcefile))

			for sourcefile in g_sources:
				ext = extension(sourcefile)
				boilerplate = "solo_mutatee_boilerplate" + ext
				basename = sourcefile[0:-len('_mutatee') - len(ext)]

				out.write("%s_mutatee_solo%s%s: ../src/%s\n"
						% (basename, cto, ObjSuffix, boilerplate))
				out.write("\t$(M_%s) %s -DTEST_NAME=%s -DGROUPABLE=1 -DMUTATEE_SRC=../src/%s -Fo$@ $**\n"
						% (compiler['defstring'],
						   object_flag_string(platform, compiler, abi, o),
						   basename, sourcefile))

			for l in compiler['languages']:
				lang = find_language(l) # Get language dictionary from name
				for e in lang['extensions']:
					# FIXME This generates spurious lines for compilers that
					# aren't used for this part of the mutatee build system
					# like .s and .S files for gcc and Fortran files.
					out.write("%%%s%s: ../src/%%%s\n"
							  % (cto, ObjSuffix, e))
					out.write("\t$(M_%s) %s -Fo$@ $**\n"
							  % (compiler['defstring'], object_flag_string(platform, compiler, abi, o)))
	out.write("\n")

# Main function for generating nmake.solo_mutatee.gen
def write_make_solo_mutatee_gen_nt(filename, tuplefile):
	read_tuples(tuplefile)
	compilers = info['compilers']
	mutatees = info['mutatees']
	out = open(filename, "w")
	print_mutatee_comp_defs_nt(out)
	# vvvvv This one isn't nt specific vvvvv
	comps = collect_mutatee_comps(mutatees)
	pname = os.environ.get('PLATFORM')
	platform = find_platform(pname)
	ObjSuffix = platform['filename_conventions']['object_suffix']
	unique_target_dict = {}
	for c in comps:
		# Generate a block of rules for mutatees produced by each compiler
		muts = filter(lambda x: x['compiler'] == c, mutatees)
		print_mutatee_rules_nt(out, muts, compilers[c], unique_target_dict)
		# Print rules for exceptional object files
		# TODO: don't know if this needs to be done for windows
		#print_special_object_rules(c, out)
# TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
		print_patterns_nt(c, out)
		out.write("# Rules for building the driver and utility objects\n")
		# TODO Replace this code generation with language neutral code
		# generation
		if 'c' in info['compilers'][c]['languages']:
			for abi in platform['abis']:
				out.write("mutatee_driver_solo_%s_%s%s: ../src/mutatee_driver.c\n" % (c, abi, ObjSuffix))
				out.write("\t$(M_%s) %s %s %s -Dsnprintf=_snprintf -Fo$@ -c $**\n"
						  % (compilers[c]['defstring'],
							 compilers[c]['flags']['std'],
							 compilers[c]['flags']['mutatee'],
							 compilers[c]['abiflags'][platform['name']][abi]))
				out.write("mutatee_util_%s_%s%s: ../src/mutatee_util.c\n"
						  % (c,
							 abi, ObjSuffix))
				out.write("\t$(M_%s) %s %s %s -Dsnprintf=_snprintf -Fo$@ -c $**\n\n"
						  % (compilers[c]['defstring'],
							 compilers[c]['flags']['std'],
							 compilers[c]['flags']['mutatee'],
							 compilers[c]['abiflags'][platform['name']][abi]))
		else:
			out.write("# (Skipped: driver and utility objects cannot be compiled with this compiler\n")

	# Print pattern rules for this platform's auxilliary compilers
	#print_aux_patterns(out, platform, comps)

	# Print footer (list of targets, clean rules, compiler presence #defines)
	print_make_solo_mutatee_gen_footer(out, comps, platform)

	out.write("mutatees: $(SOLO_MUTATEES)\n");

	out.close()
