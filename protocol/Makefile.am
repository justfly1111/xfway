ACLOCAL_AMFLAGS = -I m4

unstable_protocols =								\
xfway-shell.xml							\
	$(NULL)

stable_protocols =								\
						\
	$(NULL)

nobase_dist_pkgdata_DATA =							\
	$(unstable_protocols)							\
	$(stable_protocols)							\
	$(NULL)

#dist_noinst_DATA =								\
	#$(sort $(foreach p,$(unstable_protocols),$(dir $p)README))		\
	#$(sort $(foreach p,$(stable_protocols),$(dir $p)README))		\
#	$(NULL)

#noarch_pkgconfig_DATA = wayland-protocols.pc

dist_check_SCRIPTS = scan.sh

TESTS = $(unstable_protocols) $(stable_protocols)
TEST_EXTENSIONS = .xml
AM_TESTS_ENVIRONMENT = SCANNER='$(wayland_scanner)'; export SCANNER;
XML_LOG_COMPILER = $(srcdir)/scan.sh
