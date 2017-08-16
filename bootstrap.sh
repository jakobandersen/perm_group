subdirs="include src test"

function indentAndSlash {
	cat | sort | \
		sed "s/^/	/" |	\
		sed "s/$/ \\\\/" |	\
		sed "\$s/\\\\//"
}

function gen_root {
	echo "SUBDIRS = $subdirs"
	echo ""
	echo "EXTRA_DIST = \\"
	(
		find example -type f
		find src -type f
		find test -type f
	) | indentAndSlash
}

function gen_include {
	echo "nobase_include_HEADERS = \\"
	find perm_group -type f | indentAndSlash
}

function gen_src {
	cat <<-EOF
	lib_LTLIBRARIES = libperm_group.la
	libperm_group_la_CPPFLAGS = \$(AM_CPPFLAGS)
 	libperm_group_la_CXXFLAGS = \$(AM_CXXFLAGS)
 	libperm_group_la_LDFLAGS = \$(AM_LDFLAGS) -no-undefined
 	libperm_group_la_LIBADD = \$(AM_LDLIBS)
	libperm_group_la_SOURCES = \\
	EOF
	find . -iname "*.cpp" | indentAndSlash
}

function gen_test {
	echo "@VALGRIND_CHECK_RULES@"
	echo "check_PROGRAMS = \\"
	ls *.cpp | sed "s/.cpp$//" | indentAndSlash
	echo "TESTS = \$(check_PROGRAMS)"
	for f in $(ls *.cpp); do
		e=${f%.cpp}
		echo "${e}_SOURCES = $f"
		echo "${e}_CPPFLAGS = \$(AM_CPPFLAGS) -fsanitize=undefined -fsanitize=address"
		echo "${e}_LDADD = ../src/libperm_group.la"
		echo "${e}_LDADD += -lboost_program_options -lboost_system" # -lboost_random -lboost_chrono"
		echo "${e}_LDFLAGS = -fsanitize=undefined -fsanitize=address"
	done
}

echo "VERSION"
git describe --tags --always > VERSION
echo "Makefile.am"
gen_root > Makefile.am
for d in $subdirs; do
	echo $d/Makefile.am
	cd $d 
	$(echo "gen_$d") > Makefile.am
	cd ..
done
autoreconf -fi
